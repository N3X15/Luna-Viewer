--[[ 
  @file llview.cpp
  @author James Cook
  @brief Container for other views, anything that draws.
 
  $LicenseInfo:firstyear=2001&license=viewergpl$
  
  Copyright (c) 2001-2009, Linden Research, Inc.
  
  Second Life Viewer Source Code
  The source code in this file ("Source Code") is provided by Linden Lab
  to you under the terms of the GNU General Public License, version 2.0
  ("GPL"), unless you have obtained a separate licensing agreement
  ("Other License"), formally executed by you and Linden Lab.  Terms of
  the GPL can be found in doc/GPL-license.txt in this distribution, or
  online at http:--secondlifegrid.net/programs/open_source/licensing/gplv2
  
  There are special exceptions to the terms and conditions of the GPL as
  it is applied to this Source Code. View the full text of the exception
  in the file doc/FLOSS-exception.txt in this software distribution, or
  online at
  http:--secondlifegrid.net/programs/open_source/licensing/flossexception
  
  By copying, modifying or distributing this software, you acknowledge
  that you have read and understood your obligations described above,
  and agree to abide by those obligations.
  
  ALL LINDEN LAB SOURCE CODE IS PROVIDED "AS IS." LINDEN LAB MAKES NO
  WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
  COMPLETENESS OR PERFORMANCE.
  $/LicenseInfo$
 ]]--
 
 -- Static settings
LLView={
	sDebugRects = false,
	sDebugKeys = false,
	sDepth = 0,
	sDebugMouseHandling = false,
	sMouseHandlerMessage,
	sEditingUI = false,
	sForceReshape = false,
	sEditingUIView = nil,
	sLastLeftXML = _MIN,
	sLastBottomXML = _MIN,
end

function LLView:new(name, rect, mouse_opaque, reshape,o)
	self:mParentView=nil
	self:mName=name
	self:mRect=rect
	self:mBoundingRect=rect
	self:mReshapeFlags=reshape
	self:mDefaultTabGroup=0
	self:mEnabled=true
	self:mMouseOpaque=mouse_opaque
	self:mSoundFlags=MOUSE_UP -- default to only make sound on mouse up
	self:mSaveToXML=true
	self:mIsFocusRoot=false
	self:mLastVisible=true
	self:mUseBoundingRect=false
	self:mVisible=true
	self:mNextInsertionOrdinal=0
	self:mHoverCursor=UI_CURSOR_ARROW
	self:mDelayedDelete=false
end

-- virtual
function LLView:isView() 
	return true
end

-- virtual
function LLView:isCtrl() 
	return false
end

-- virtual
function LLView:isPanel() 
	return false
end

-- virtual
function LLView:setToolTip(  msg)
	mToolTipMsg = msg
end

function LLView:setToolTipArg(key, text)
	mToolTipMsg.setArg(key, text)
	return true
end

function LLView:setToolTipArgs(  LLStringUtil::format_map_t& args )
	mToolTipMsg.setArgList(args)
end

-- virtual
function LLView:setRect( LLRect& rect)
	mRect = rect
	updateBoundingRect()
end

function LLView:setUseBoundingRect( use_bounding_rect ) 
	if (mUseBoundingRect ~= use_bounding_rect) then
        mUseBoundingRect = use_bounding_rect 
		updateBoundingRect()
	end
end

function LLView:getUseBoundingRect()
	return mUseBoundingRect
end

-- virtual
function LLView:getName() 
	static   unnamed("(no name)")
	return mName.empty() ? unnamed : mName
end

function LLView:sendChildToFront(LLView* child)
	if (child && child->getParent() == this) then
		mChildList.remove( child )
		mChildList.push_front(child)
	end
end

function LLView:sendChildToBack(LLView* child)
	if (child && child->getParent() == this) then
		mChildList.remove( child )
		mChildList.push_back(child)
	end
end

function LLView:moveChildToFrontOfTabGroup(LLUICtrl* child)
	if(mCtrlOrder.find(child) ~= mCtrlOrder.end())then
		mCtrlOrder[child].second = -1 * mNextInsertionOrdinal++
	end
end

function LLView:moveChildToBackOfTabGroup(LLUICtrl* child)
	if(mCtrlOrder.find(child) ~= mCtrlOrder.end())then
		mCtrlOrder[child].second = mNextInsertionOrdinal++
	end
end

function LLView:addChild(LLView* child,  tab_group)
	if (mParentView == child) then
		llerrs << "Adding view " << child->getName() << " as child of itself" << llendl
	end
	-- remove from current parent
	if (child->mParentView) then
		child->mParentView->removeChild(child)
	end

	-- add to front of child list, as normal
	mChildList.push_front(child)

	-- add to ctrl list if is LLUICtrl
	if (child->isCtrl())then
		-- controls are stored in reverse order from render order
		addCtrlAtEnd((LLUICtrl*) child, tab_group)
	end

	child->mParentView = this
	updateBoundingRect()
end


function LLView:addChildAtEnd(LLView* child,  tab_group)
	if (mParentView == child) then
		llerrs << "Adding view " << child->getName() << " as child of itself" << llendl
	end
	-- remove from current parent
	if (child->mParentView) then
		child->mParentView->removeChild(child)
	end

	-- add to back of child list
	mChildList.push_back(child)

	-- add to ctrl list if is LLUICtrl
	if (child->isCtrl())then
		-- controls are stored in reverse order from render order
		addCtrl((LLUICtrl*) child, tab_group)
	end
	
	child->mParentView = this
	updateBoundingRect()
end

-- remove the specified child from the view, and set it's parent to nil.
function LLView:removeChild(LLView* child, deleteIt)
	if (child->mParentView == this) then
		mChildList.remove( child )
		child->mParentView = nil
		if (child->isCtrl())
		{
			removeCtrl((LLUICtrl*)child)
		end
		if (deleteIt)
		{
			delete child
		end
	end
	elsethen
		llerrs << "LLView:removeChild called with non-child" << llendl
	end
	updateBoundingRect()
end

function LLView:addCtrlAtEnd(LLUICtrl* ctrl,  tab_group)
	mCtrlOrder.insert(tab_order_pair_t(ctrl,
								tab_order_t(tab_group, mNextInsertionOrdinal++)))
end

function LLView:addCtrl( LLUICtrl* ctrl,  tab_group)
	-- add to front of list by using negative ordinal, which monotonically increases
	mCtrlOrder.insert(tab_order_pair_t(ctrl,
								tab_order_t(tab_group, -1 * mNextInsertionOrdinal++)))
end

function LLView:removeCtrl(LLUICtrl* ctrl)
	child_tab_order_t::iterator found = mCtrlOrder.find(ctrl)
	if(found ~= mCtrlOrder.end())then
		mCtrlOrder.erase(found)
	end
end

function LLView:getCtrlList() 
	controls={}
	for(c in pairs(self:mChildList)) do
		if (iter:isCtrl()) then
			controls+=iter
		end
	end
	return controls
end

function LLView:getCtrlListSorted() 
	return sort(getCtrlList())
end


-- This method compares two LLViews by the tab order specified in the comparator object.  The
-- code for this is a little convoluted because each argument can have four states:
-- 1) not a control, 2) a control but not in the tab order, 3) a control in the tab order, 4) nil
function LLCompareByTabOrder() (a,b) 
	 a_score = 0, b_score = 0
	if(a ~= nil) then a_score=a_score-1 end
	if(b ~= nil) then b_score=b_score-1 end
	if(a && a.isCtrl()) then a_score=a_score-1 end
	if(b && b.isCtrl()) then b_score=b_score-1 end
	if(a_score == -2 and b_score == -2)then
		a_found = mTabOrder.find(a)
		b_found = mTabOrder.find(b)
		if (a_found ~= mTabOrder.end()) then a_score=a_score-1 end
		if (b_found ~= mTabOrder.end()) then b_score=b_score-1 end
		if (a_score == -3 and b_score == -3) then
			-- whew!  Once we're in here, they're both in the tab order, and we can compare based on that
			return compareTabOrders(mTabOrder[a_found],mTabOrder[b_found])
		end
	end
	return (a_score == b_score) ? a < b : a_score < b_score
end

LLView:isInVisibleChain() 
	 LLView* cur_view = this
	while(cur_view)then
		if (!cur_view->getVisible())
		{
			return false
		end
		cur_view = cur_view->getParent()
	end
	return true
end

LLView:isInEnabledChain() 
	 LLView* cur_view = this
	while(cur_view)then
		if (!cur_view->getEnabled())
		{
			return false
		end
		cur_view = cur_view->getParent()
	end
	return true
end

-- virtual
LLView:canFocusChildren() 
	return true
end

--virtual
function LLView:setTentative(b)
end

--virtual
LLView:getTentative() 
	return false
end

--virtual
function LLView:setEnabled(enabled)
	mEnabled = enabled
end

--virtual
LLView:setLabelArg(  & key,   text )
	return false
end

--virtual
LLRect LLView:getSnapRect() 
	return mRect
end

--virtual
LLRect LLView:getRequiredRect()
	return mRect
end

--virtual
function LLView:onFocusLost()
end

--virtual
function LLView:onFocusReceived()
end

LLView:focusNextRoot()
	LLView:child_list_t result = LLView:getFocusRootsQuery().run(this)
	return LLView:focusNext(result)
end

LLView:focusPrevRoot()
	LLView:child_list_t result = LLView:getFocusRootsQuery().run(this)
	return LLView:focusPrev(result)
end

-- static
LLView:focusNext(LLView:child_list_t & result)
	LLView:child_list_iter_t focused = result.end()
	for(LLView:child_list_iter_t iter = result.begin()
		iter ~= result.end()
		++iter)then
		if(gFocusMgr.childHasKeyboardFocus(*iter))
		{
			focused = iter
			break
		end
	end
	LLView:child_list_iter_t next = focused
	next = (next == result.end()) ? result.begin() : ++next
	while(next ~= focused)then
		-- wrap around to beginning if necessary
		if(next == result.end())
		{
			next = result.begin()
		end
		if((*next)->isCtrl())
		{
			LLUICtrl * ctrl = static_cast<LLUICtrl*>(*next)
			ctrl->setFocus(true)
			ctrl->onTabInto()  
			gFocusMgr.triggerFocusFlash()
			return true
		end
		++next
	end
	return false
end

-- static
LLView:focusPrev(LLView:child_list_t & result)
	LLView:child_list_reverse_iter_t focused = result.rend()
	for(LLView:child_list_reverse_iter_t iter = result.rbegin()
		iter ~= result.rend()
		++iter)then
		if(gFocusMgr.childHasKeyboardFocus(*iter))
		{
			focused = iter
			break
		end
	end
	LLView:child_list_reverse_iter_t next = focused
	next = (next == result.rend()) ? result.rbegin() : ++next
	while(next ~= focused)then
		-- wrap around to beginning if necessary
		if(next == result.rend())
		{
			next = result.rbegin()
		end
		if((*next)->isCtrl())
		{
			LLUICtrl * ctrl = static_cast<LLUICtrl*>(*next)
			if (!ctrl->hasFocus())
			{
				ctrl->setFocus(true)
				ctrl->onTabInto()  
				gFocusMgr.triggerFocusFlash()
			end
			return true
		end
		++next
	end
	return false
end

-- delete all children. Override this function if you need to
-- perform any extra clean up such as cached pointers to selected
-- children, etc.
-- <edit>
--function LLView:deleteAllChildren()
function LLView:deleteAllChildren(delay_delete)
-- </edit>
	-- <edit> TESTZONE DERF
	if(delay_delete)then
		child_list_t::iterator end = mChildList.end()
		for(child_list_t::iterator iter = mChildList.begin() iter ~= end ++iter)
		{
			if((*iter)->getParent() == this)
				(*iter)->mParentView = nil
		end
		mCtrlOrder.clear()
		std::list<LLView*> children(mChildList)
		gDeleteScheduler->addViewDeleteJob(children)
		return
	end
	-- </edit>
	-- clear out the control ordering
	mCtrlOrder.clear()

	while (!mChildList.empty())then
		LLView* viewp = mChildList.front()
		delete viewp -- will remove the child from mChildList
	end
end

function LLView:setAllChildrenEnabled(b)
	for ( child_list_iter_t child_it = mChildList.begin() child_it ~= mChildList.end() ++child_it)then
		LLView* viewp = *child_it
		viewp->setEnabled(b)
	end
end

-- virtual
function LLView:setVisible(visible)
	if ( mVisible ~= visible )then
		if( !visible && (gFocusMgr.getTopCtrl() == this) )
		{
			gFocusMgr.setTopCtrl( nil )
		end

		mVisible = visible

		-- notify children of visibility change if root, or part of visible hierarchy
		if (!getParent() || getParent()->isInVisibleChain())
		{
			-- tell all children of this view that the visibility may have changed
			onVisibilityChange( visible )
		end
		updateBoundingRect()
	end
end

-- virtual
function LLView:onVisibilityChange ( new_visibility )
	for ( child_list_iter_t child_it = mChildList.begin() child_it ~= mChildList.end() ++child_it)then
		LLView* viewp = *child_it
		-- only views that are themselves visible will have their overall visibility affected by their ancestors
		if (viewp->getVisible())
		{
			viewp->onVisibilityChange ( new_visibility )
		end
	end
end

-- virtual
function LLView:translate( x,  y)
	mRect.translate(x, y)
	updateBoundingRect()
end

-- virtual
LLView:canSnapTo( LLView* other_view)
	return other_view ~= this && other_view->getVisible()
end

-- virtual
function LLView:snappedTo( LLView* snap_view)
end

LLView:handleHover( x,  y, MASK mask)
	handled = childrenHandleHover( x, y, mask ) ~= nil
	if( !handled 
		&& blockMouseEvent(x, y) )then
		LLUI::sWindow->setCursor(mHoverCursor)
		lldebugst(LLERR_USER_INPUT) << "hover handled by " << getName() << llendl
		handled = true
	end

	return handled
end

 LLView:getShowNamesToolTip()
	LLView* view = getParent()
	 name
	 tool_tip = mName

	while (view)then
		name = view->getName()

		if (name == "root") break

		if (view->getToolTip().find(".xml") ~= ::npos)
		{
			tool_tip = view->getToolTip() + "/" +  tool_tip
			break
		end
		else
		{
			tool_tip = view->getName() + "/" +  tool_tip
		end

		view = view->getParent()
	end

	return "/" + tool_tip
end


LLView:handleToolTip( x,  y, & msg, LLRect* sticky_rect_screen)
	handled = false

	 tool_tip

	for ( child_list_iter_t child_it = mChildList.begin() child_it ~= mChildList.end() ++child_it)then
		LLView* viewp = *child_it
		 local_x = x - viewp->mRect.mLeft
		 local_y = y - viewp->mRect.mBottom
		-- Allow tooltips for disabled views so we can explain to the user why
		-- the view is disabled. JC
		if( viewp->pointInView(local_x, local_y) 
			&& viewp->getVisible() 
			-- && viewp->getEnabled()
			&& viewp->handleToolTip(local_x, local_y, msg, sticky_rect_screen ))
		{
			-- child provided a tooltip, just return
			if (!msg.empty()) return true

			-- otherwise, one of our children ate the event so don't traverse
			-- siblings however, our child did not actually provide a tooltip
            -- so we might want to
			handled = true
			break
		end
	end

	-- get our own tooltip
	tool_tip = mToolTipMsg.getString()
	if (
		LLUI::sShowXUINames 
		&& (tool_tip.find(".xml", 0) == ::npos) 
		&& (mName.find("Drag", 0) == ::npos))then
		tool_tip = getShowNamesToolTip()
	end

	show_names_text_box = LLUI::sShowXUINames && dynamic_cast<LLTextBox*>(this) ~= nil

	-- don't allow any siblings to handle this event
	-- even if we don't have a tooltip
	if (blockMouseEvent(x, y) || show_names_text_box)then
		if(!tool_tip.empty())
		{
			msg = tool_tip

			-- Convert rect local to screen coordinates
			localPointToScreen(
				0, 0,
				&(sticky_rect_screen->mLeft), &(sticky_rect_screen->mBottom) )
			localPointToScreen(
				mRect.getWidth(), mRect.getHeight(),
				&(sticky_rect_screen->mRight), &(sticky_rect_screen->mTop) )		
		end
		handled = true
	end

	return handled
end

LLView:handleKey(KEY key, MASK mask, called_from_parent)
	handled = false

	if (getVisible() && getEnabled())then
		if( called_from_parent )
		{
			-- Downward traversal
			handled = childrenHandleKey( key, mask ) ~= nil
		end

		if (!handled)
		{
			handled = handleKeyHere( key, mask )
			if (handled && LLView:sDebugKeys)
			{
				llinfos << "Key handled by " << getName() << llendl
			end
		end
	end

	if( !handled && !called_from_parent && mParentView)then
		-- Upward traversal
		handled = mParentView->handleKey( key, mask, false )
	end
	return handled
end

-- Called from handleKey()
-- Handles key in this object.  Checking parents and children happens in handleKey()
LLView:handleKeyHere(KEY key, MASK mask)
	return false
end

LLView:handleUnicodeChar(llwchar uni_char, called_from_parent)
	handled = false

	if (getVisible() && getEnabled())then
		if( called_from_parent )
		{
			-- Downward traversal
			handled = childrenHandleUnicodeChar( uni_char ) ~= nil
		end

		if (!handled)
		{
			handled = handleUnicodeCharHere(uni_char)
			if (handled && LLView:sDebugKeys)
			{
				llinfos << "Unicode key handled by " << getName() << llendl
			end
		end
	end

	if (!handled && !called_from_parent && mParentView)then
		-- Upward traversal
		handled = mParentView->handleUnicodeChar(uni_char, false)
	end

	return handled
end


LLView:handleUnicodeCharHere(llwchar uni_char )
	return false
end


LLView:handleDragAndDrop( x,  y, MASK mask, drop,
							   EDragAndDropType cargo_type, function* cargo_data,
							   EAcceptance* accept,
							   & tooltip_msg)
	-- CRO this is an experiment to allow drag and drop into object inventory based on the DragAndDrop tool's permissions rather than the parent
	handled = childrenHandleDragAndDrop( x, y, mask, drop,
											cargo_type,
											cargo_data,
											accept,
											tooltip_msg) ~= nil
	if( !handled && blockMouseEvent(x, y) )then
		*accept = ACCEPT_NO
		handled = true
		lldebugst(LLERR_USER_INPUT) << "dragAndDrop handled by LLView " << getName() << llendl
	end

	return handled
end

LLView* LLView:childrenHandleDragAndDrop( x,  y, MASK mask,
									   drop,
									   EDragAndDropType cargo_type,
									   function* cargo_data,
									   EAcceptance* accept,
									   & tooltip_msg)
	LLView* handled_view = false
	-- CRO this is an experiment to allow drag and drop into object inventory based on the DragAndDrop tool's permissions rather than the parent
	if( getVisible() )
--	if( getVisible() && getEnabled() )then
		for ( child_list_iter_t child_it = mChildList.begin() child_it ~= mChildList.end() ++child_it)
		{
			LLView* viewp = *child_it
			 local_x = x - viewp->getRect().mLeft
			 local_y = y - viewp->getRect().mBottom
			if( viewp->pointInView(local_x, local_y) && 
				viewp->getVisible() &&
				viewp->getEnabled() &&
				viewp->handleDragAndDrop(local_x, local_y, mask, drop,
										 cargo_type,
										 cargo_data,
										 accept,
										 tooltip_msg))
			{
				handled_view = viewp
				break
			end
		end
	end
	return handled_view
end

function LLView:onMouseCaptureLost()
end

LLView:hasMouseCapture() 
	return gFocusMgr.getMouseCapture() == this 
end

LLView:handleMouseUp( x,  y, MASK mask)
	handled = childrenHandleMouseUp( x, y, mask ) ~= nil
	if( !handled && blockMouseEvent(x, y) )then
		handled = true
	end
	return handled
end

LLView:handleMouseDown( x,  y, MASK mask)
	LLView* handled_view = childrenHandleMouseDown( x, y, mask )
	handled = (handled_view ~= nil)
	if( !handled && blockMouseEvent(x, y) )then
		handled = true
		handled_view = this
	end

	-- HACK If we're editing UI, select the leaf view that ate the click.
	if (LLView:sEditingUI && handled_view)then
		-- need to find leaf views, big hack
		LLButton* buttonp = dynamic_cast<LLButton*>(handled_view)
		LLLineEditor* line_editorp = dynamic_cast<LLLineEditor*>(handled_view)
		LLTextEditor* text_editorp = dynamic_cast<LLTextEditor*>(handled_view)
		LLTextBox* text_boxp = dynamic_cast<LLTextBox*>(handled_view)
		if (buttonp
			|| line_editorp
			|| text_editorp
			|| text_boxp)
		{
			LLView:sEditingUIView = handled_view
		end
	end

	return handled
end

LLView:handleDoubleClick( x,  y, MASK mask)
	handled = childrenHandleDoubleClick( x, y, mask ) ~= nil
	if( !handled && blockMouseEvent(x, y) )then
		handleMouseDown(x, y, mask)
		handled = true
	end
	return handled
end

LLView:handleScrollWheel( x,  y,  clicks)
	handled = false
	if( getVisible() && getEnabled() )then
		handled = childrenHandleScrollWheel( x, y, clicks ) ~= nil
		if( !handled && blockMouseEvent(x, y) )
		{
			handled = true
		end
	end
	return handled
end

LLView:handleRightMouseDown( x,  y, MASK mask)
	handled = childrenHandleRightMouseDown( x, y, mask ) ~= nil
	if( !handled && blockMouseEvent(x, y) )then
		handled = true
	end
	return handled
end

LLView:handleRightMouseUp( x,  y, MASK mask)
	handled = childrenHandleRightMouseUp( x, y, mask ) ~= nil
	if( !handled && blockMouseEvent(x, y) )then
		handled = true
	end
	return handled
end
 
LLView:handleMiddleMouseDown( x,  y, MASK mask)
	LLView* handled_view = childrenHandleMiddleMouseDown( x, y, mask )
	handled = (handled_view ~= nil)
	if( !handled && blockMouseEvent(x, y) )then
		handled = true
		handled_view = this
	end

	return handled
end

LLView:handleMiddleMouseUp( x,  y, MASK mask)
	handled = childrenHandleMiddleMouseUp( x, y, mask ) ~= nil
	if( !handled && blockMouseEvent(x, y) )then
		handled = true
	end
	return handled
end


LLView* LLView:childrenHandleScrollWheel( x,  y,  clicks)
	LLView* handled_view = nil
	if (getVisible() && getEnabled() )then
		for ( child_list_iter_t child_it = mChildList.begin() child_it ~= mChildList.end() ++child_it)
		{
			LLView* viewp = *child_it
			 local_x = x - viewp->getRect().mLeft
			 local_y = y - viewp->getRect().mBottom
			if (viewp->pointInView(local_x, local_y) 
				&& viewp->getVisible()
				&& viewp->getEnabled()
				&& viewp->handleScrollWheel( local_x, local_y, clicks ))
			{
				if (LLView:sDebugMouseHandling)
				{
					LLView:sMouseHandlerMessage = ("->") + viewp->mName + LLView:sMouseHandlerMessage
				end

				handled_view = viewp
				break
			end
		end
	end
	return handled_view
end

LLView* LLView:childrenHandleHover( x,  y, MASK mask)
	LLView* handled_view = nil
	if (getVisible() && getEnabled() )then
		for ( child_list_iter_t child_it = mChildList.begin() child_it ~= mChildList.end() ++child_it)
		{
			LLView* viewp = *child_it
			 local_x = x - viewp->getRect().mLeft
			 local_y = y - viewp->getRect().mBottom
			if(viewp->pointInView(local_x, local_y) &&
				viewp->getVisible() &&
				viewp->getEnabled() &&
				viewp->handleHover(local_x, local_y, mask) )
			{
				if (LLView:sDebugMouseHandling)
				{
					LLView:sMouseHandlerMessage = ("->") + viewp->mName + LLView:sMouseHandlerMessage
				end

				handled_view = viewp
				break
			end
		end
	end
	return handled_view
end

-- Called during downward traversal
LLView* LLView:childrenHandleKey(KEY key, MASK mask)
	LLView* handled_view = nil

	if ( getVisible() && getEnabled() )then
		for ( child_list_iter_t child_it = mChildList.begin() child_it ~= mChildList.end() ++child_it)
		{
			LLView* viewp = *child_it
			if (viewp->handleKey(key, mask, true))
			{
				if (LLView:sDebugKeys)
				{
					llinfos << "Key handled by " << viewp->getName() << llendl
				end
				handled_view = viewp
				break
			end
		end
	end

	return handled_view
end

-- Called during downward traversal
LLView* LLView:childrenHandleUnicodeChar(llwchar uni_char)
	LLView* handled_view = nil

	if ( getVisible() && getEnabled() )then
		for ( child_list_iter_t child_it = mChildList.begin() child_it ~= mChildList.end() ++child_it)
		{
			LLView* viewp = *child_it
			if (viewp->handleUnicodeChar(uni_char, true))
			{
				if (LLView:sDebugKeys)
				{
					llinfos << "Unicode character handled by " << viewp->getName() << llendl
				end
				handled_view = viewp
				break
			end
		end
	end

	return handled_view
end

LLView* LLView:childrenHandleMouseDown( x,  y, MASK mask)
	LLView* handled_view = nil

	for ( child_list_iter_t child_it = mChildList.begin() child_it ~= mChildList.end() ++child_it)then
		LLView* viewp = *child_it
		 local_x = x - viewp->getRect().mLeft
		 local_y = y - viewp->getRect().mBottom

		if (viewp->pointInView(local_x, local_y) && 
			viewp->getVisible() && 
			viewp->getEnabled() && 
			viewp->handleMouseDown( local_x, local_y, mask ))
		{
			if (LLView:sDebugMouseHandling)
			{
				LLView:sMouseHandlerMessage = ("->") + viewp->mName + LLView:sMouseHandlerMessage
			end
			handled_view = viewp
			break
		end
	end
	return handled_view
end

LLView* LLView:childrenHandleRightMouseDown( x,  y, MASK mask)
	LLView* handled_view = nil

	if (getVisible() && getEnabled() )then
		for ( child_list_iter_t child_it = mChildList.begin() child_it ~= mChildList.end() ++child_it)
		{
			LLView* viewp = *child_it
			 local_x = x - viewp->getRect().mLeft
			 local_y = y - viewp->getRect().mBottom
			if (viewp->pointInView(local_x, local_y) &&
				viewp->getVisible() &&
				viewp->getEnabled() &&
				viewp->handleRightMouseDown( local_x, local_y, mask ))
			{
				if (LLView:sDebugMouseHandling)
				{
					LLView:sMouseHandlerMessage = ("->") + viewp->mName + LLView:sMouseHandlerMessage
				end
				handled_view = viewp
				break
			end
		end
	end
	return handled_view
end

LLView* LLView:childrenHandleMiddleMouseDown( x,  y, MASK mask)
	LLView* handled_view = nil

	if (getVisible() && getEnabled() )then
		for ( child_list_iter_t child_it = mChildList.begin() child_it ~= mChildList.end() ++child_it)
		{
			LLView* viewp = *child_it
			 local_x = x - viewp->getRect().mLeft
			 local_y = y - viewp->getRect().mBottom
			if (viewp->pointInView(local_x, local_y) &&
				viewp->getVisible() &&
				viewp->getEnabled() &&
				viewp->handleMiddleMouseDown( local_x, local_y, mask ))
			{
				if (LLView:sDebugMouseHandling)
				{
					LLView:sMouseHandlerMessage = ("->") + viewp->mName + LLView:sMouseHandlerMessage
				end
				handled_view = viewp
				break
			end
		end
	end
	return handled_view
end

LLView* LLView:childrenHandleDoubleClick( x,  y, MASK mask)
	LLView* handled_view = nil

	if (getVisible() && getEnabled() )then
		for ( child_list_iter_t child_it = mChildList.begin() child_it ~= mChildList.end() ++child_it)
		{
			LLView* viewp = *child_it
			 local_x = x - viewp->getRect().mLeft
			 local_y = y - viewp->getRect().mBottom
			if (viewp->pointInView(local_x, local_y) &&
				viewp->getVisible() &&
				viewp->getEnabled() &&
				viewp->handleDoubleClick( local_x, local_y, mask ))
			{
				if (LLView:sDebugMouseHandling)
				{
					LLView:sMouseHandlerMessage = ("->") + viewp->mName + LLView:sMouseHandlerMessage
				end
				handled_view = viewp
				break
			end
		end
	end
	return handled_view
end

LLView* LLView:childrenHandleMouseUp( x,  y, MASK mask)
	LLView* handled_view = nil
	if( getVisible() && getEnabled() )then
		for ( child_list_iter_t child_it = mChildList.begin() child_it ~= mChildList.end() ++child_it)
		{
			LLView* viewp = *child_it
			 local_x = x - viewp->getRect().mLeft
			 local_y = y - viewp->getRect().mBottom
			if (!viewp->pointInView(local_x, local_y))
				continue
			if (!viewp->getVisible())
				continue
			if (!viewp->getEnabled())
				continue
			if (viewp->handleMouseUp( local_x, local_y, mask ))
			{
				if (LLView:sDebugMouseHandling)
				{
					LLView:sMouseHandlerMessage = ("->") + viewp->mName + LLView:sMouseHandlerMessage
				end
				handled_view = viewp
				break
			end
		end
	end
	return handled_view
end

LLView* LLView:childrenHandleRightMouseUp( x,  y, MASK mask)
	LLView* handled_view = nil
	if( getVisible() && getEnabled() )then
		for ( child_list_iter_t child_it = mChildList.begin() child_it ~= mChildList.end() ++child_it)
		{
			LLView* viewp = *child_it
			 local_x = x - viewp->getRect().mLeft
			 local_y = y - viewp->getRect().mBottom
			if (viewp->pointInView(local_x, local_y) &&
				viewp->getVisible() &&
				viewp->getEnabled() &&
				viewp->handleRightMouseUp( local_x, local_y, mask ))
			{
				if (LLView:sDebugMouseHandling)
				{
					LLView:sMouseHandlerMessage = ("->") + viewp->mName + LLView:sMouseHandlerMessage
				end
				handled_view = viewp
				break
			end
		end
	end
	return handled_view
end

LLView* LLView:childrenHandleMiddleMouseUp( x,  y, MASK mask)
	LLView* handled_view = nil
	if( getVisible() && getEnabled() )then
		for ( child_list_iter_t child_it = mChildList.begin() child_it ~= mChildList.end() ++child_it)
		{
			LLView* viewp = *child_it
			 local_x = x - viewp->getRect().mLeft
			 local_y = y - viewp->getRect().mBottom
			if (viewp->pointInView(local_x, local_y) &&
				viewp->getVisible() &&
				viewp->getEnabled() &&
				viewp->handleMiddleMouseUp( local_x, local_y, mask ))
			{
				if (LLView:sDebugMouseHandling)
				{
					LLView:sMouseHandlerMessage = ("->") + viewp->mName + LLView:sMouseHandlerMessage
				end
				handled_view = viewp
				break
			end
		end
	end
	return handled_view
end

function LLView:draw()
	if (LLView:sDebugRects)then
		drawDebugRect()

		-- Check for bogus rectangle
		if (getRect().mRight <= getRect().mLeft
			|| getRect().mTop <= getRect().mBottom)
		{
			llwarns << "Bogus rectangle for " << getName() << " with " << mRect << llendl
		end
	end

	LLRect rootRect = getRootView()->getRect()
	LLRect screenRect

	-- draw focused control on top of everything else
	LLUICtrl* focus_view = dynamic_cast<LLUICtrl*>(gFocusMgr.getKeyboardFocus())
	if (focus_view && focus_view->getParent() ~= this)then
		focus_view = nil
	end

	++LLView:sDepth
	for (child_list_reverse_iter_t child_iter = mChildList.rbegin() child_iter ~= mChildList.rend() ++child_iter)then
		LLView *viewp = *child_iter

		if (viewp->getVisible() && viewp ~= focus_view && viewp->getRect().isValid())
		{
			-- Only draw views that are within the root view
			localRectToScreen(viewp->getRect(),&screenRect)
			if ( rootRect.rectInRect(&screenRect) )
			{
				glMatrixMode(GL_MODELVIEW)
				LLUI::pushMatrix()
				{
					LLUI::translate((F32)viewp->getRect().mLeft, (F32)viewp->getRect().mBottom, 0.f)
					viewp->draw()
				end
				LLUI::popMatrix()
			end
		end

	end
	--LLView:sDepth

	if (focus_view && focus_view->getVisible())then
		drawChild(focus_view)
	end

	-- HACK
	if (LLView:sEditingUI && this == LLView:sEditingUIView)then
		drawDebugRect()
	end
end

--Draw a box for debugging.
function LLView:drawDebugRect()
	LLUI::pushMatrix()then
		-- drawing solids requires texturing be disabled
		gGL.getTexUnit(0)->unbind(LLTexUnit::TT_TEXTURE)

		if (mUseBoundingRect)
		{
			LLUI::translate((F32)mBoundingRect.mLeft - (F32)mRect.mLeft, (F32)mBoundingRect.mBottom - (F32)mRect.mBottom, 0.f)
		end

		LLRect debug_rect = mUseBoundingRect ? mBoundingRect : mRect

		-- draw red rectangle for the border
		LLColor4 border_color(0.f, 0.f, 0.f, 1.f)
		if (LLView:sEditingUI)
		{
			border_color.mV[0] = 1.f
		end
		else
		{
			border_color.mV[LLView:sDepth%3] = 1.f
		end

		gGL.color4fv( border_color.mV )

		gGL.begin(LLRender::LINES)
			gGL.vertex2i(0, debug_rect.getHeight() - 1)
			gGL.vertex2i(0, 0)

			gGL.vertex2i(0, 0)
			gGL.vertex2i(debug_rect.getWidth() - 1, 0)

			gGL.vertex2i(debug_rect.getWidth() - 1, 0)
			gGL.vertex2i(debug_rect.getWidth() - 1, debug_rect.getHeight() - 1)

			gGL.vertex2i(debug_rect.getWidth() - 1, debug_rect.getHeight() - 1)
			gGL.vertex2i(0, debug_rect.getHeight() - 1)
		gGL.end()

		-- Draw the name if it's not a leaf node
		if (mChildList.size() && !LLView:sEditingUI)
		{
			--char temp[256]
			 x, y
			gGL.color4fv( border_color.mV )
			x = debug_rect.getWidth()/2
			y = debug_rect.getHeight()/2
			 debug_text = llformat("%s (%d x %d)", getName().c_str(),
										debug_rect.getWidth(), debug_rect.getHeight())
			LLFontGL::getFontSansSerifSmall()->renderUTF8(debug_text, 0, (F32)x, (F32)y, border_color,
												LLFontGL::HCENTER, LLFontGL::BASELINE, LLFontGL::NORMAL,
												_MAX, _MAX, nil, false)
		end
	end
	LLUI::popMatrix()
end

function LLView:drawChild(LLView* childp,  x_offset,  y_offset, force_draw)
	if (childp && childp->getParent() == this)then
		++LLView:sDepth

		if ((childp->getVisible() && childp->getRect().isValid()) 
			|| force_draw)
		{
			glMatrixMode(GL_MODELVIEW)
			LLUI::pushMatrix()
			{
				LLUI::translate((F32)childp->getRect().mLeft + x_offset, (F32)childp->getRect().mBottom + y_offset, 0.f)
				childp->draw()
			end
			LLUI::popMatrix()
		end

		--LLView:sDepth
	end
end


function LLView:reshape( width,  height, called_from_parent)
	-- compute how much things changed and apply reshape logic to children
	 delta_width = width - getRect().getWidth()
	 delta_height = height - getRect().getHeight()

	if (delta_width || delta_height || LLView:sForceReshape)then
		-- adjust our rectangle
		mRect.mRight = getRect().mLeft + width
		mRect.mTop = getRect().mBottom + height

		-- move child views according to reshape flags
		for ( child_list_iter_t child_it = mChildList.begin() child_it ~= mChildList.end() ++child_it)
		{
			LLView* viewp = *child_it
			LLRect child_rect( viewp->mRect )

			if (viewp->followsRight() && viewp->followsLeft())
			{
				child_rect.mRight += delta_width
			end
			else if (viewp->followsRight())
			{
				child_rect.mLeft += delta_width
				child_rect.mRight += delta_width
			end
			else if (viewp->followsLeft())
			{
				-- left is 0, don't need to adjust coords
			end
			else
			{
				-- BUG what to do when we don't follow anyone?
				-- for now, same as followsLeft
			end

			if (viewp->followsTop() && viewp->followsBottom())
			{
				child_rect.mTop += delta_height
			end
			else if (viewp->followsTop())
			{
				child_rect.mTop += delta_height
				child_rect.mBottom += delta_height
			end
			else if (viewp->followsBottom())
			{
				-- bottom is 0, so don't need to adjust coords
			end
			else
			{
				-- BUG what to do when we don't follow?
				-- for now, same as bottom
			end

			 delta_x = child_rect.mLeft - viewp->getRect().mLeft
			 delta_y = child_rect.mBottom - viewp->getRect().mBottom
			viewp->translate( delta_x, delta_y )
			viewp->reshape(child_rect.getWidth(), child_rect.getHeight())
		end
	end

	if (!called_from_parent)then
		if (mParentView)
		{
			mParentView->reshape(mParentView->getRect().getWidth(), mParentView->getRect().getHeight(), false)
		end
	end

	updateBoundingRect()
end

function LLView:updateBoundingRect()
	if (isDead()) return

	if (mUseBoundingRect)then
		LLRect local_bounding_rect = LLRect::nil

		child_list__iter_t child_it
		for ( child_it = mChildList.begin() child_it ~= mChildList.end() ++child_it)
		{
			LLView* childp = *child_it
			-- ignore invisible and "top" children when calculating bounding rect
			-- such as combobox popups
			if (!childp->getVisible() || childp == gFocusMgr.getTopCtrl()) 
			{
				continue
			end

			LLRect child_bounding_rect = childp->getBoundingRect()

			if (local_bounding_rect.isnil())
			{
				-- start out with bounding rect equal to first visible child's bounding rect
				local_bounding_rect = child_bounding_rect
			end
			else
			{
				-- accumulate non-nil children rectangles
				if (!child_bounding_rect.isnil())
				{
					local_bounding_rect.unionWith(child_bounding_rect)
				end
			end
		end

		mBoundingRect = local_bounding_rect
		-- translate into parent-relative coordinates
		mBoundingRect.translate(mRect.mLeft, mRect.mBottom)
	end
	elsethen
		mBoundingRect = mRect
	end

	-- give parent view a chance to resize, in case we just moved, for example
	if (getParent() && getParent()->mUseBoundingRect)then
		getParent()->updateBoundingRect()
	end
end

LLRect LLView:getScreenRect() 
	-- *FIX: check for one-off error
	LLRect screen_rect
	localPointToScreen(0, 0, &screen_rect.mLeft, &screen_rect.mBottom)
	localPointToScreen(getRect().getWidth(), getRect().getHeight(), &screen_rect.mRight, &screen_rect.mTop)
	return screen_rect
end

LLRect LLView:getLocalBoundingRect() 
	LLRect local_bounding_rect = getBoundingRect()
	local_bounding_rect.translate(-mRect.mLeft, -mRect.mBottom)

	return local_bounding_rect
end


LLRect LLView:getLocalRect() 
	LLRect local_rect(0, getRect().getHeight(), getRect().getWidth(), 0)
	return local_rect
end

LLRect LLView:getLocalSnapRect() 
	LLRect local_snap_rect = getSnapRect()
	local_snap_rect.translate(-getRect().mLeft, -getRect().mBottom)
	return local_snap_rect
end

LLView:hasAncestor( LLView* parentp) 
	if (!parentp)then
		return false
	end

	LLView* viewp = getParent()
	while(viewp)then
		if (viewp == parentp)
		{
			return true
		end
		viewp = viewp->getParent()
	end

	return false
end

-------------------------------------------------------------------------------

LLView:childHasKeyboardFocus(  & childname ) 
	LLView *child = getChildView(childname, true, false)
	if (child)then
		return gFocusMgr.childHasKeyboardFocus(child)
	end
	elsethen
		return false
	end
end

-------------------------------------------------------------------------------

LLView:hasChild( & childname, recurse) 
	return getChildView(childname, recurse, false) ~= nil
end

-------------------------------------------------------------------------------
-- getChildView()
-------------------------------------------------------------------------------
LLView* LLView:getChildView( & name, recurse, create_if_missing) 
	--richard: should we allow empty names?
	--if(name.empty())
	--	return nil
	child_list__iter_t child_it
	-- Look for direct children *first*
	for ( child_it = mChildList.begin() child_it ~= mChildList.end() ++child_it)then
		LLView* childp = *child_it
		if (childp->getName() == name)
		{
			return childp
		end
	end
	if (recurse)then
		-- Look inside each child as well.
		for ( child_it = mChildList.begin() child_it ~= mChildList.end() ++child_it)
		{
			LLView* childp = *child_it
			LLView* viewp = childp->getChildView(name, recurse, false)
			if ( viewp )
			{
				return viewp
			end
		end
	end

	if (create_if_missing)then
		return createDummyWidget<LLView>(name)
	end
	return nil
end

LLView:parentPointInView( x,  y, EHitTestType type)   
	return (mUseBoundingRect && type == HIT_TEST_USE_BOUNDING_RECT)
		? mBoundingRect.pointInRect( x, y ) 
		: mRect.pointInRect( x, y ) 
end

LLView:pointInView( x,  y, EHitTestType type)   
	return (mUseBoundingRect && type == HIT_TEST_USE_BOUNDING_RECT)
		? mBoundingRect.pointInRect( x + mRect.mLeft, y + mRect.mBottom ) 
		: mRect.localPointInRect( x, y ) 
end

LLView:blockMouseEvent( x,  y) 
	return mMouseOpaque && pointInView(x, y, HIT_TEST_IGNORE_BOUNDING_RECT)
end

-- virtual
function LLView:screenPointToLocal( screen_x,  screen_y, * local_x, * local_y) 
	*local_x = screen_x - getRect().mLeft
	*local_y = screen_y - getRect().mBottom

	 LLView* cur = this
	while( cur->mParentView )then
		cur = cur->mParentView
		*local_x -= cur->getRect().mLeft
		*local_y -= cur->getRect().mBottom
	end
end

function LLView:localPointToScreen( local_x,  local_y, * screen_x, * screen_y) 
	*screen_x = local_x + getRect().mLeft
	*screen_y = local_y + getRect().mBottom

	 LLView* cur = this
	while( cur->mParentView )then
		cur = cur->mParentView
		*screen_x += cur->getRect().mLeft
		*screen_y += cur->getRect().mBottom
	end
end

function LLView:screenRectToLocal( LLRect& screen, LLRect* local) 
	*local = screen
	local->translate( -getRect().mLeft, -getRect().mBottom )

	 LLView* cur = this
	while( cur->mParentView )then
		cur = cur->mParentView
		local->translate( -cur->getRect().mLeft, -cur->getRect().mBottom )
	end
end

function LLView:localRectToScreen( LLRect& local, LLRect* screen) 
	*screen = local
	screen->translate( getRect().mLeft, getRect().mBottom )

	 LLView* cur = this
	while( cur->mParentView )then
		cur = cur->mParentView
		screen->translate( cur->getRect().mLeft, cur->getRect().mBottom )
	end
end

LLView* LLView:getRootView()
	LLView* view = this
	while( view->mParentView )then
		view = view->mParentView
	end
	return view
end

LLView:deleteViewByHandle(LLHandle<LLView> handle)
	LLView* viewp = handle.get()

	--<edit> Getting some crashes on exit,
	--guess it should check if the view actually exists before deleting it, but I'm not wading through all of Philip's code.
	--probably a bug in the delayed ui delete crap (what *is* that, anyways?)
	if(viewp)
		delete viewp
	--</edit>

	return viewp ~= nil
end


-- Moves the view so that it is entirely inside of raint.
-- If the view will not fit because it's too big, aligns with the top and left.
-- (Why top and left?  That's where the drag bars are for floaters.)
LLView:translateIntoRect( LLRect& raint, allow_partial_outside )
	 delta_x = 0
	 delta_y = 0

	if (allow_partial_outside)then
		  KEEP_ONSCREEN_PIXELS = 16

		if( getRect().mRight - KEEP_ONSCREEN_PIXELS < raint.mLeft )
		{
			delta_x = raint.mLeft - (getRect().mRight - KEEP_ONSCREEN_PIXELS)
		end
		else
		if( getRect().mLeft + KEEP_ONSCREEN_PIXELS > raint.mRight )
		{
			delta_x = raint.mRight - (getRect().mLeft + KEEP_ONSCREEN_PIXELS)
		end

		if( getRect().mTop > raint.mTop )
		{
			delta_y = raint.mTop - getRect().mTop
		end
		else
		if( getRect().mTop - KEEP_ONSCREEN_PIXELS < raint.mBottom )
		{
			delta_y = raint.mBottom - (getRect().mTop - KEEP_ONSCREEN_PIXELS)
		end
	end
	elsethen
		if( getRect().mLeft < raint.mLeft )
		{
			delta_x = raint.mLeft - getRect().mLeft
		end
		else
		if( getRect().mRight > raint.mRight )
		{
			delta_x = raint.mRight - getRect().mRight
			-- compensate for left edge possible going off screen
			delta_x += llmax( 0, getRect().getWidth() - raint.getWidth() )
		end

		if( getRect().mTop > raint.mTop )
		{
			delta_y = raint.mTop - getRect().mTop
		end
		else
		if( getRect().mBottom < raint.mBottom )
		{
			delta_y = raint.mBottom - getRect().mBottom
			-- compensate for top edge possible going off screen
			delta_y -= llmax( 0, getRect().getHeight() - raint.getHeight() )
		end
	end

	if (delta_x ~= 0 || delta_y ~= 0)then
		translate(delta_x, delta_y)
		return true
	end
	return false
end

function LLView:centerWithin( LLRect& bounds)
	 left   = bounds.mLeft + (bounds.getWidth() - getRect().getWidth()) / 2
	 bottom = bounds.mBottom + (bounds.getHeight() - getRect().getHeight()) / 2

	translate( left - getRect().mLeft, bottom - getRect().mBottom )
end

LLView:localPointToOtherView(  x,  y,  *other_x,  *other_y, LLView* other_view) 
	 LLView* cur_view = this
	 LLView* root_view = nil

	while (cur_view)then
		if (cur_view == other_view)
		{
			*other_x = x
			*other_y = y
			return true
		end

		x += cur_view->getRect().mLeft
		y += cur_view->getRect().mBottom

		cur_view = cur_view->getParent()
		root_view = cur_view
	end

	-- assuming common root between two views, chase other_view's parents up to root
	cur_view = other_view
	while (cur_view)then
		x -= cur_view->getRect().mLeft
		y -= cur_view->getRect().mBottom

		cur_view = cur_view->getParent()

		if (cur_view == root_view)
		{
			*other_x = x
			*other_y = y
			return true
		end
	end

	*other_x = x
	*other_y = y
	return false
end

LLView:localRectToOtherView(  LLRect& local, LLRect* other, LLView* other_view ) 
	LLRect cur_rect = local
	 LLView* cur_view = this
	 LLView* root_view = nil

	while (cur_view)then
		if (cur_view == other_view)
		{
			*other = cur_rect
			return true
		end

		cur_rect.translate(cur_view->getRect().mLeft, cur_view->getRect().mBottom)

		cur_view = cur_view->getParent()
		root_view = cur_view
	end

	-- assuming common root between two views, chase other_view's parents up to root
	cur_view = other_view
	while (cur_view)then
		cur_rect.translate(-cur_view->getRect().mLeft, -cur_view->getRect().mBottom)

		cur_view = cur_view->getParent()

		if (cur_view == root_view)
		{
			*other = cur_rect
			return true
		end
	end

	*other = cur_rect
	return false
end

-- virtual
LLXMLNodePtr LLView:getXML(save_children) 
	--If called from a derived class, the derived class will override the node name
	LLXMLNodePtr node = new LLXMLNode("view", false)

	node->createChild("name", true)->setStringValue(getName())
	node->createChild("width", true)->setIntValue(getRect().getWidth())
	node->createChild("height", true)->setIntValue(getRect().getHeight())

	LLView* parent = getParent()
	 left = getRect().mLeft
	 bottom = getRect().mBottom
	if (parent) bottom -= parent->getRect().getHeight()

	node->createChild("left", true)->setIntValue(left)
	node->createChild("bottom", true)->setIntValue(bottom)

	U32 follows_flags = getFollows()
	if (follows_flags)then
		stream buffer
		pipe = false
		if (followsLeft())
		{
			buffer << "left"
			pipe = true
		end
		if (followsTop())
		{
			if (pipe) buffer << "|"
			buffer << "top"
			pipe = true
		end
		if (followsRight())
		{
			if (pipe) buffer << "|"
			buffer << "right"
			pipe = true
		end
		if (followsBottom())
		{
			if (pipe) buffer << "|"
			buffer << "bottom"
		end
		node->createChild("follows", true)->setStringValue(buffer.str())
	end
	-- Export all widgets as enabled and visible - code must disable.
	node->createChild("mouse_opaque", true)->setValue(mMouseOpaque )
	if (!mToolTipMsg.getString().empty())then
		node->createChild("tool_tip", true)->setStringValue(mToolTipMsg.getString())
	end
	if (mSoundFlags ~= MOUSE_UP)then
		node->createChild("sound_flags", true)->setIntValue(()mSoundFlags)
	end

	node->createChild("enabled", true)->setValue(getEnabled())

	if (!mControlName.empty())then
		node->createChild("control_name", true)->setStringValue(mControlName)
	end
	return node
end

--static 
LLView* LLView:fromXML(LLXMLNodePtr node, LLView *parent, LLUICtrlFactory *factory)
	LLView* viewp = new LLView()
	viewp->initFromXML(node, parent)
	return viewp
end

-- static
function LLView:addColorXML(LLXMLNodePtr node,  LLColor4& color,
							 char* xml_name,  char* control_name)
	if (color ~= LLUI::sColorsGroup->getColor(ll_safe_string(control_name)))then
		node->createChild(xml_name, true)->setFloatValue(4, color.mV)
	end
end

--static 
 LLView:escapeXML( & xml, & indent)
	 ret = indent + "\"" + LLXMLNode::escapeXML(xml)

	--replace every newline with a close quote, new line, indent, open quote
	size_t index = ret.size()-1
	size_t fnd
	
	while ((fnd = ret.rfind("\n", index)) ~= ::npos)then
		ret.replace(fnd, 1, "\"\n" + indent + "\"")
		index = fnd-1
	end

	--append close quote
	ret.append("\"")
	
	return ret	
end

-- static
LLWString LLView:escapeXML( LLWString& xml)
	LLWString out
	for (LLWString::size_type i = 0 i < xml.size() ++i)then
		llwchar c = xml[i]
		switch(c)
		{
		case '"':	out.append(utf8string_to_wstring("&quot"))	break
		case '\'':	out.append(utf8string_to_wstring("&apos"))	break
		case '&':	out.append(utf8string_to_wstring("&amp"))		break
		case '<':	out.append(utf8string_to_wstring("&lt"))		break
		case '>':	out.append(utf8string_to_wstring("&gt"))		break
		default:	out.push_back(c) break
		end
	end
	return out
end

-- static
 LLCtrlQuery & LLView:getTabOrderQuery()
	static LLCtrlQuery query
	if(query.getPreFilters().size() == 0) {
		query.addPreFilter(LLVisibleFilter::getInstance())
		query.addPreFilter(LLEnabledFilter::getInstance())
		query.addPreFilter(LLTabStopFilter::getInstance())
		query.addPostFilter(LLLeavesFilter::getInstance())
	end
	return query
end

-- This class is only used internally by getFocusRootsQuery below. 
class LLFocusRootsFilter : public LLQueryFilter, public LLSingleton<LLFocusRootsFilter>
	--[[virtual]]-- filterResult_t operator() ( LLView*  view,  viewList_t & children)  then
		return filterResult_t(view->isCtrl() && view->isFocusRoot(), !view->isFocusRoot())
	end
end

-- static
 LLCtrlQuery & LLView:getFocusRootsQuery()
	static LLCtrlQuery query
	if(query.getPreFilters().size() == 0) {
		query.addPreFilter(LLVisibleFilter::getInstance())
		query.addPreFilter(LLEnabledFilter::getInstance())
		query.addPreFilter(LLFocusRootsFilter::getInstance())
		query.addPostFilter(LLRootsFilter::getInstance())
	end
	return query
end


function	LLView:userSetShape( LLRect& new_rect)
	reshape(new_rect.getWidth(), new_rect.getHeight())
	translate(new_rect.mLeft - getRect().mLeft, new_rect.mBottom - getRect().mBottom)
end

LLView* LLView:findSnapRect(LLRect& new_rect,  LLCoordGL& mouse_dir,
							 LLView:ESnapType snap_type,  threshold,  padding)
	new_rect = mRect
	LLView* snap_view = nil

	if (!mParentView)then
		return nil
	end
	
	 delta_x = 0
	 delta_y = 0
	if (mouse_dir.mX >= 0)then
		 new_right = mRect.mRight
		LLView* view = findSnapEdge(new_right, mouse_dir, SNAP_RIGHT, snap_type, threshold, padding)
		delta_x = new_right - mRect.mRight
		snap_view = view ? view : snap_view
	end

	if (mouse_dir.mX <= 0)then
		 new_left = mRect.mLeft
		LLView* view = findSnapEdge(new_left, mouse_dir, SNAP_LEFT, snap_type, threshold, padding)
		delta_x = new_left - mRect.mLeft
		snap_view = view ? view : snap_view
	end

	if (mouse_dir.mY >= 0)then
		 new_top = mRect.mTop
		LLView* view = findSnapEdge(new_top, mouse_dir, SNAP_TOP, snap_type, threshold, padding)
		delta_y = new_top - mRect.mTop
		snap_view = view ? view : snap_view
	end

	if (mouse_dir.mY <= 0)then
		 new_bottom = mRect.mBottom
		LLView* view = findSnapEdge(new_bottom, mouse_dir, SNAP_BOTTOM, snap_type, threshold, padding)
		delta_y = new_bottom - mRect.mBottom
		snap_view = view ? view : snap_view
	end

	new_rect.translate(delta_x, delta_y)
	return snap_view
end

LLView*	LLView:findSnapEdge(& new_edge_val,  LLCoordGL& mouse_dir, ESnapEdge snap_edge, ESnapType snap_type,  threshold,  padding)
	LLRect snap_rect = getSnapRect()
	 snap_pos = 0
	switch(snap_edge)then
	case SNAP_LEFT:
		snap_pos = snap_rect.mLeft
		break
	case SNAP_RIGHT:
		snap_pos = snap_rect.mRight
		break
	case SNAP_TOP:
		snap_pos = snap_rect.mTop
		break
	case SNAP_BOTTOM:
		snap_pos = snap_rect.mBottom
		break
	end

	if (!mParentView)then
		new_edge_val = snap_pos
		return nil
	end

	LLView* snap_view = nil

	-- If the view is near the edge of its parent, snap it to
	-- the edge.
	LLRect test_rect = snap_rect
	test_rect.stretch(padding)

	 x_threshold = threshold
	 y_threshold = threshold

	LLRect parent_local_snap_rect = mParentView->getLocalSnapRect()

	if (snap_type == SNAP_PARENT || snap_type == SNAP_PARENT_AND_SIBLINGS)then
		switch(snap_edge)
		{
		case SNAP_RIGHT:
			if (llabs(parent_local_snap_rect.mRight - test_rect.mRight) <= x_threshold 
				&& (parent_local_snap_rect.mRight - test_rect.mRight) * mouse_dir.mX >= 0)
			{
				snap_pos = parent_local_snap_rect.mRight - padding
				snap_view = mParentView
				x_threshold = llabs(parent_local_snap_rect.mRight - test_rect.mRight)
			end
			break
		case SNAP_LEFT:
			if (llabs(test_rect.mLeft - parent_local_snap_rect.mLeft) <= x_threshold 
				&& test_rect.mLeft * mouse_dir.mX <= 0)
			{
				snap_pos = parent_local_snap_rect.mLeft + padding
				snap_view = mParentView
				x_threshold = llabs(test_rect.mLeft - parent_local_snap_rect.mLeft)
			end
			break
		case SNAP_BOTTOM:
			if (llabs(test_rect.mBottom - parent_local_snap_rect.mBottom) <= y_threshold 
				&& test_rect.mBottom * mouse_dir.mY <= 0)
			{
				snap_pos = parent_local_snap_rect.mBottom + padding
				snap_view = mParentView
				y_threshold = llabs(test_rect.mBottom - parent_local_snap_rect.mBottom)
			end
			break
		case SNAP_TOP:
			if (llabs(parent_local_snap_rect.mTop - test_rect.mTop) <= y_threshold && (parent_local_snap_rect.mTop - test_rect.mTop) * mouse_dir.mY >= 0)
			{
				snap_pos = parent_local_snap_rect.mTop - padding
				snap_view = mParentView
				y_threshold = llabs(parent_local_snap_rect.mTop - test_rect.mTop)
			end
			break
		default:
			llerrs << "Invalid snap edge" << llendl
		end
	end

	if (snap_type == SNAP_SIBLINGS || snap_type == SNAP_PARENT_AND_SIBLINGS)then
		for ( child_list__iter_t child_it = mParentView->getChildList()->begin()
			  child_it ~= mParentView->getChildList()->end() ++child_it)
		{
			LLView* siblingp = *child_it

			if (!canSnapTo(siblingp)) continue

			LLRect sibling_rect = siblingp->getSnapRect()

			switch(snap_edge)
			{
			case SNAP_RIGHT:
				if (llabs(test_rect.mRight - sibling_rect.mLeft) <= x_threshold 
					&& (test_rect.mRight - sibling_rect.mLeft) * mouse_dir.mX <= 0)
				{
					snap_pos = sibling_rect.mLeft - padding
					snap_view = siblingp
					x_threshold = llabs(test_rect.mRight - sibling_rect.mLeft)
				end
				-- if snapped with sibling along other axis, check for shared edge
				else if (llabs(sibling_rect.mTop - (test_rect.mBottom - padding)) <= y_threshold 
					|| llabs(sibling_rect.mBottom - (test_rect.mTop + padding)) <= x_threshold)
				{
					if (llabs(test_rect.mRight - sibling_rect.mRight) <= x_threshold 
						&& (test_rect.mRight - sibling_rect.mRight) * mouse_dir.mX <= 0)
					{
						snap_pos = sibling_rect.mRight
						snap_view = siblingp
						x_threshold = llabs(test_rect.mRight - sibling_rect.mRight)
					end
				end
				break
			case SNAP_LEFT:
				if (llabs(test_rect.mLeft - sibling_rect.mRight) <= x_threshold 
					&& (test_rect.mLeft - sibling_rect.mRight) * mouse_dir.mX <= 0)
				{
					snap_pos = sibling_rect.mRight + padding
					snap_view = siblingp
					x_threshold = llabs(test_rect.mLeft - sibling_rect.mRight)				
				end
				-- if snapped with sibling along other axis, check for shared edge
				else if (llabs(sibling_rect.mTop - (test_rect.mBottom - padding)) <= y_threshold 
					|| llabs(sibling_rect.mBottom - (test_rect.mTop + padding)) <= y_threshold)
				{
					if (llabs(test_rect.mLeft - sibling_rect.mLeft) <= x_threshold 
						&& (test_rect.mLeft - sibling_rect.mLeft) * mouse_dir.mX <= 0)
					{
						snap_pos = sibling_rect.mLeft
						snap_view = siblingp
						x_threshold = llabs(test_rect.mLeft - sibling_rect.mLeft)									
					end
				end
				break
			case SNAP_BOTTOM:
				if (llabs(test_rect.mBottom - sibling_rect.mTop) <= y_threshold 
					&& (test_rect.mBottom - sibling_rect.mTop) * mouse_dir.mY <= 0)
				{
					snap_pos = sibling_rect.mTop + padding
					snap_view = siblingp
					y_threshold = llabs(test_rect.mBottom - sibling_rect.mTop)
				end
				-- if snapped with sibling along other axis, check for shared edge
				else if (llabs(sibling_rect.mRight - (test_rect.mLeft - padding)) <= x_threshold 
					|| llabs(sibling_rect.mLeft - (test_rect.mRight + padding)) <= x_threshold)
				{
					if (llabs(test_rect.mBottom - sibling_rect.mBottom) <= y_threshold 
						&& (test_rect.mBottom - sibling_rect.mBottom) * mouse_dir.mY <= 0)
					{
						snap_pos = sibling_rect.mBottom
						snap_view = siblingp
						y_threshold = llabs(test_rect.mBottom - sibling_rect.mBottom)
					end
				end
				break
			case SNAP_TOP:
				if (llabs(test_rect.mTop - sibling_rect.mBottom) <= y_threshold 
					&& (test_rect.mTop - sibling_rect.mBottom) * mouse_dir.mY <= 0)
				{
					snap_pos = sibling_rect.mBottom - padding
					snap_view = siblingp
					y_threshold = llabs(test_rect.mTop - sibling_rect.mBottom)
				end
				-- if snapped with sibling along other axis, check for shared edge
				else if (llabs(sibling_rect.mRight - (test_rect.mLeft - padding)) <= x_threshold 
					|| llabs(sibling_rect.mLeft - (test_rect.mRight + padding)) <= x_threshold)
				{
					if (llabs(test_rect.mTop - sibling_rect.mTop) <= y_threshold 
						&& (test_rect.mTop - sibling_rect.mTop) * mouse_dir.mY <= 0)
					{
						snap_pos = sibling_rect.mTop
						snap_view = siblingp
						y_threshold = llabs(test_rect.mTop - sibling_rect.mTop)
					end
				end
				break
			default:
				llerrs << "Invalid snap edge" << llendl
			end
		end
	end

	new_edge_val = snap_pos
	return snap_view
end

-------------------------------------------------------------------------------
-- Listener dispatch functions
-------------------------------------------------------------------------------

function LLView:registerEventListener( name, LLSimpleListener* function)
	mDispatchList.insert(std::pair<, LLSimpleListener*>(name, function))
	lldebugs << getName() << " registered " << name << llendl

end

function LLView:deregisterEventListener( name)
	dispatch_list_t::iterator itor = mDispatchList.find(name)
	if (itor ~= mDispatchList.end())then
		mDispatchList.erase(itor)
	end
end

 LLView:findEventListener(LLSimpleListener *listener) 
	dispatch_list_t::_iterator itor
	for (itor = mDispatchList.begin() itor ~= mDispatchList.end() ++itor)then
		if (itor->second == listener)
		{
			return itor->first
		end
	end
	if (mParentView)then
		return mParentView->findEventListener(listener)
	end
	return LLStringUtil::nil
end

LLSimpleListener* LLView:getListenerByName( & callback_name)
	LLSimpleListener* callback = nil
	dispatch_list_t::iterator itor = mDispatchList.find(callback_name)
	if (itor ~= mDispatchList.end())then
		callback = itor->second
	end
	else if (mParentView)then
		callback = mParentView->getListenerByName(callback_name)
	end
	return callback
end

LLControlVariable *LLView:findControl( & name)
	control_map_t::iterator itor = mFloaterControls.find(name)
	if (itor ~= mFloaterControls.end())then
		return itor->second
	end
	if (mParentView)then
		return mParentView->findControl(name)
	end
	return LLUI::sConfigGroup->getControl(name)
end

  FLOATER_H_MARGIN = 15
  MIN_WIDGET_HEIGHT = 10
  VPAD = 4

-- static
U32 LLView:createRect(LLXMLNodePtr node, LLRect &rect, LLView* parent_view,  LLRect &required_rect)
	U32 follows = 0
	 x = rect.mLeft
	 y = rect.mBottom
	 w = rect.getWidth()
	 h = rect.getHeight()

	U32 last_x = 0
	U32 last_y = 0
	if (parent_view)then
		last_y = parent_view->getRect().getHeight()
		child_list_t::_iterator itor = parent_view->getChildList()->begin()
		if (itor ~= parent_view->getChildList()->end())
		{
			LLView *last_view = (*itor)
			if (last_view->getSaveToXML())
			{
				last_x = last_view->getRect().mLeft
				last_y = last_view->getRect().mBottom
			end
		end
	end

	 rect_control
	node->getAttributeString("rect_control", rect_control)
	if (! rect_control.empty())then
		LLRect rect = LLUI::sConfigGroup->getRect(rect_control)
		x = rect.mLeft
		y = rect.mBottom
		w = rect.getWidth()
		h = rect.getHeight()
	end
	
	if (node->hasAttribute("left"))then
		node->getAttribute("left", x)
	end
	if (node->hasAttribute("bottom"))then
		node->getAttribute("bottom", y)
	end

	-- Make your width the width of the containing
	-- view if you don't specify a width.
	if (parent_view)then
		if(w == 0)
		{
			w = llmax(required_rect.getWidth(), parent_view->getRect().getWidth() - (FLOATER_H_MARGIN) - x)
		end

		if(h == 0)
		{
			h = llmax(MIN_WIDGET_HEIGHT, required_rect.getHeight())
		end
	end

	if (node->hasAttribute("width"))then
		node->getAttribute("width", w)
	end
	if (node->hasAttribute("height"))then
		node->getAttribute("height", h)
	end

	if (parent_view)then
		if (node->hasAttribute("left_delta"))
		{
			 left_delta = 0
			node->getAttribute("left_delta", left_delta)
			x = last_x + left_delta
		end
		else if (node->hasAttribute("left") && node->hasAttribute("right"))
		{
			-- compute width based on left and right
			 right = 0
			node->getAttribute("right", right)
			if (right < 0)
			{
				right = parent_view->getRect().getWidth() + right
			end
			w = right - x
		end
		else if (node->hasAttribute("left"))
		{
			if (x < 0)
			{
				x = parent_view->getRect().getWidth() + x
				follows |= FOLLOWS_RIGHT
			end
			else
			{
				follows |= FOLLOWS_LEFT
			end
		end
		else if (node->hasAttribute("width") && node->hasAttribute("right"))
		{
			 right = 0
			node->getAttribute("right", right)
			if (right < 0)
			{
				right = parent_view->getRect().getWidth() + right
			end
			x = right - w
		end
		else
		{
			-- left not specified, same as last
			x = last_x
		end

		if (node->hasAttribute("bottom_delta"))
		{
			 bottom_delta = 0
			node->getAttribute("bottom_delta", bottom_delta)
			y = last_y + bottom_delta
		end
		else if (node->hasAttribute("top"))
		{
			-- compute height based on top
			 top = 0
			node->getAttribute("top", top)
			if (top < 0)
			{
				top = parent_view->getRect().getHeight() + top
			end
			h = top - y
		end
		else if (node->hasAttribute("bottom"))
		{
			if (y < 0)
			{
				y = parent_view->getRect().getHeight() + y
				follows |= FOLLOWS_TOP
			end
			else
			{
				follows |= FOLLOWS_BOTTOM
			end
		end
		else
		{
			-- if bottom not specified, generate automatically
			if (last_y == 0)
			{
				-- treat first child as "bottom"
				y = parent_view->getRect().getHeight() - (h + VPAD)
				follows |= FOLLOWS_TOP
			end
			else
			{
				-- treat subsequent children as "bottom_delta"
				y = last_y - (h + VPAD)
			end
		end
	end
	elsethen
		x = llmax(x, 0)
		y = llmax(y, 0)
		follows = FOLLOWS_LEFT | FOLLOWS_TOP
	end
	rect.setOriginAndSize(x, y, w, h)

	return follows
end

function LLView:initFromXML(LLXMLNodePtr node, LLView* parent)
	-- create rect first, as this will supply initial follows flags
	LLRect view_rect
	U32 follows_flags = createRect(node, view_rect, parent, getRequiredRect())
	-- call reshape in case there are any child elements that need to be layed out
	reshape(view_rect.getWidth(), view_rect.getHeight())
	setRect(view_rect)
	setFollows(follows_flags)

	parseFollowsFlags(node)

	if (node->hasAttribute("control_name"))then
		 control_name
		node->getAttributeString("control_name", control_name)
		setControlName(control_name, nil)
	end

	if (node->hasAttribute("tool_tip"))then
		 tool_tip_msg
		node->getAttributeString("tool_tip", tool_tip_msg)
		setToolTip(tool_tip_msg)
	end

	if (node->hasAttribute("enabled"))then
		enabled
		node->getAttribute("enabled", enabled)
		setEnabled(enabled)
	end
	
	if (node->hasAttribute("visible"))then
		visible
		node->getAttribute("visible", visible)
		setVisible(visible)
	end

	if (node->hasAttribute("hover_cursor"))then
		 cursor_string
		node->getAttributeString("hover_cursor", cursor_string)
		mHoverCursor = getCursorFromString(cursor_string)
	end
	
	node->getAttribute("use_bounding_rect", mUseBoundingRect)
	node->getAttribute("mouse_opaque", mMouseOpaque)

	node->getAttribute("default_tab_group", mDefaultTabGroup)
	
	reshape(view_rect.getWidth(), view_rect.getHeight())
end

function LLView:parseFollowsFlags(LLXMLNodePtr node)
	if (node->hasAttribute("follows"))then
		setFollowsNone()

		 follows
		node->getAttributeString("follows", follows)

		typedef boost::tokenizer<boost::char_separator<char> > tokenizer
		boost::char_separator<char> sep("|")
		tokenizer tokens(follows, sep)
		tokenizer::iterator token_iter = tokens.begin()

		while(token_iter ~= tokens.end())
		{
			 & token_str = *token_iter
			if (token_str == "left")
			{
				setFollowsLeft()
			end
			else if (token_str == "right")
			{
				setFollowsRight()
			end
			else if (token_str == "top")
			{
				setFollowsTop()
			end
			else if (token_str == "bottom")
			{
				setFollowsBottom()
			end
			else if (token_str == "all")
			{
				setFollowsAll()
			end
			++token_iter
		end
	end
end

-- static
LLFontGL* LLView:selectFont(LLXMLNodePtr node)
	 font_name, font_size, font_style
	U8 style = 0
	
	if (node->hasAttribute("font"))then
		node->getAttributeString("font", font_name)
	end
	
	if (node->hasAttribute("font_size"))then
		node->getAttributeString("font_size", font_size)
	end

	if (node->hasAttribute("font_style"))then
		node->getAttributeString("font_style", font_style)
		style = LLFontGL::getStyleFromString(font_style)
	end

	if (node->hasAttribute("font-style"))then
		node->getAttributeString("font-style", font_style)
		style = LLFontGL::getStyleFromString(font_style)
	end

	if (font_name.empty())
		return nil

	LLFontDescriptor desc(font_name, font_size, style)
	LLFontGL* gl_font = LLFontGL::getFont(desc)

	return gl_font
end

-- static
LLFontGL::HAlign LLView:selectFontHAlign(LLXMLNodePtr node)
	LLFontGL::HAlign gl_hfont_align = LLFontGL::LEFT

	if (node->hasAttribute("halign"))then
		 horizontal_align_name
		node->getAttributeString("halign", horizontal_align_name)
		gl_hfont_align = LLFontGL::hAlignFromName(horizontal_align_name)
	end
	return gl_hfont_align
end

-- static
LLFontGL::VAlign LLView:selectFontVAlign(LLXMLNodePtr node)
	LLFontGL::VAlign gl_vfont_align = LLFontGL::BASELINE

	if (node->hasAttribute("valign"))then
		 vert_align_name
		node->getAttributeString("valign", vert_align_name)
		gl_vfont_align = LLFontGL::vAlignFromName(vert_align_name)
	end
	return gl_vfont_align
end

-- static
LLFontGL::StyleFlags LLView:selectFontStyle(LLXMLNodePtr node)
	LLFontGL::StyleFlags gl_font_style = LLFontGL::NORMAL

	if (node->hasAttribute("style"))then
		 style_flags_name
		node->getAttributeString("style", style_flags_name)

		if (style_flags_name == "normal")
		{
			gl_font_style = LLFontGL::NORMAL
		end
		else if (style_flags_name == "bold")
		{
			gl_font_style = LLFontGL::BOLD
		end
		else if (style_flags_name == "italic")
		{
			gl_font_style = LLFontGL::ITALIC
		end
		else if (style_flags_name == "underline")
		{
			gl_font_style = LLFontGL::UNDERLINE
		end
		--else leave left
	end
	return gl_font_style
end

LLView:setControlValue( LLSD& value)
	 ctrlname = getControlName()
	if (!ctrlname.empty())then
		LLUI::sConfigGroup->setValue(ctrlname, value)
		return true
	end
	return false
end

--virtual
function LLView:setControlName( & control_name, LLView *context)
	if (context == nil)then
		context = this
	end

	if (!mControlName.empty())then
		llwarns << "setControlName called twice on same control!" << llendl
		mControlConnection.disconnect() -- disconnect current signal
		mControlName.clear()
	end
	
	-- Register new listener
	if (!control_name.empty())then
		LLControlVariable *control = context->findControl(control_name)
		if (control)
		{
			mControlName = control_name
			mControlConnection = control->getSignal()->connect(boost::bind(&controlListener, _1, getHandle(), ("value")))
			setValue(control->getValue())
		end
	end
end

-- static
LLView:controlListener( LLSD& newvalue, LLHandle<LLView> handle,  type)
	LLView* view = handle.get()
	if (view)then
		if (type == "value")
		{
			view->setValue(newvalue)
			return true
		end
		else if (type == "enabled")
		{
			view->setEnabled(newvalue.asean())
			return true
		end
		else if (type == "visible")
		{
			view->setVisible(newvalue.asean())
			return true
		end
	end
	return false
end

function LLView:addControl( & name, initial_value)
	mFloaterControls[name] = new LLControlVariable(name, TYPE_EAN, initial_value, ("Internal floater control"))
end

LLControlVariable *LLView:getControl( & name)
	control_map_t::iterator itor = mFloaterControls.find(name)
	if (itor ~= mFloaterControls.end())then
		return itor->second
	end
	return nil
end

--virtual 
function	LLView:setValue( LLSD& value)
end

--virtual 
LLSD	LLView:getValue()  
	return LLSD()
end

LLView* LLView:createWidget(LLXMLNodePtr xml_node) 
	-- forward requests to ui ctrl factory
	return LLUICtrlFactory::getInstance()->createCtrlWidget(nil, xml_node)
end

--
-- LLWidgetClassRegistry
--

LLWidgetClassRegistry::LLWidgetClassRegistry() 
end

function LLWidgetClassRegistry::registerCtrl( & tag, LLWidgetClassRegistry::factory_func_t function) 
	 lower_case_tag = tag
	LLStringUtil::toLower(lower_case_tag)
	
	mCreatorFunctions[lower_case_tag] = function
end

LLWidgetClassRegistry::isTagRegistered(  &tag) 
	return mCreatorFunctions.find(tag) ~= mCreatorFunctions.end()
end

LLWidgetClassRegistry::factory_func_t LLWidgetClassRegistry::getCreatorFunc( & ctrl_type) 
	factory_map_t::_iterator found_it = mCreatorFunctions.find(ctrl_type)
	if (found_it == mCreatorFunctions.end())then
		return nil
	end
	return found_it->second
end

