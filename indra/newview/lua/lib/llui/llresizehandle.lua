--[[
Converted by N3X15 for Luna
 * @file llresizehandle.h/cpp
 * @brief LLResizeHandle base class
 *
 * $LicenseInfo:firstyear=2001&license=viewergpl$
 * 
 * Copyright (c) 2001-2009, Linden Research, Inc.
 * 
 * Second Life Viewer Source Code
 * The source code in this file ("Source Code") is provided by Linden Lab
 * to you under the terms of the GNU General Public License, version 2.0
 * ("GPL"), unless you have obtained a separate licensing agreement
 * ("Other License"), formally executed by you and Linden Lab.  Terms of
 * the GPL can be found in doc/GPL-license.txt in this distribution, or
 * online at http://secondlifegrid.net/programs/open_source/licensing/gplv2
 * 
 * There are special exceptions to the terms and conditions of the GPL as
 * it is applied to this Source Code. View the full text of the exception
 * in the file doc/FLOSS-exception.txt in this software distribution, or
 * online at
 * http://secondlifegrid.net/programs/open_source/licensing/flossexception
 * 
 * By copying, modifying or distributing this software, you acknowledge
 * that you have read and understood your obligations described above,
 * and agree to abide by those obligations.
 * 
 * ALL LINDEN LAB SOURCE CODE IS PROVIDED "AS IS." LINDEN LAB MAKES NO
 * WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
 * COMPLETENESS OR PERFORMANCE.
 * $/LicenseInfo$
]]--

--This is what allows you to resize windows and shit.


LLResizeHandle = {
-- Constants/enums
	RESIZE_HANDLE_HEIGHT = 16
	RESIZE_HANDLE_WIDTH = 16
	LEFT_TOP=1
	LEFT_BOTTOM=2
	RIGHT_TOP=3
	RIGHT_BOTTOM=4
	
-- Local variables (NOTHING is private :M)
	mDragLastScreenX=-1
	mDragLastScreenY=-1
	mLastMouseScreenX=-1
	mLastMouseScreenY=-1
	mLastMouseDir = Vector2()
	mImage = nil -- Implement
	mMinWidth=0
	mMinHeight=0
	mCorner=RIGHT_BOTTOM
}

-- Equivilent to a constructor.  I think.
-- local durp = LLResize:new(...) (o is optional)
function LLResizeHandle:new(name, rect, min_width, min_height,corner,o)
	o = o or {}   -- create object if user does not provide one
	setmetatable(o, self)
	self.__index = self
	return o
  
	mSaveToXML=false
	if RIGHT_BOTTOM == self:mCorner then
		-- TODO: Implement LLImage bindings.
		--mImage = GetImage(LLUI,"UIImgResizeBottomRightUUID")
	then

	if self:mCorner == LEFT_TOP then	
		setFollows( FOLLOWS_LEFT | FOLLOWS_TOP )
	elseif self:mCorner == LEFT_BOTTOM then	
		setFollows( FOLLOWS_LEFT | FOLLOWS_BOTTOM )
	elseif self:mCorner == RIGHT_TOP then
		setFollows( FOLLOWS_RIGHT | FOLLOWS_TOP )
	elseif self:mCorner == RIGHT_BOTTOM then
		setFollows( FOLLOWS_RIGHT | FOLLOWS_BOTTOM )
	end
end

function LLResizeHandle:handleMouseUp(x,y,mask) 
	handled = FALSE

	if self:hasMouseCapture() then
		-- Release the mouse (I hope this is the right way to call global variables)
		gFocusMgr:setMouseCapture(nil)
		handled = TRUE
	elseif pointInHandle(x, y) then
		handled = TRUE
	end

	return handled
end


function LLResizeHandle:handleHover(x, y, mask)
	handled = false

	-- We only handle the click if the click both started and ended within us
	if self:hasMouseCapture() then
		-- Make sure the mouse in still over the application.  We don't want to make the parent
		-- so big that we can't see the resize handle any more.
	
		screen_x=0
		screen_y=0
		screen_x, screen_y = localPointToScreen(x, y)
		valid_rect = self:getRootView():getRect()
		screen_x = clamp( screen_x, valid_rect.mLeft, valid_rect.mRight )
		screen_y = clamp( screen_y, valid_rect.mBottom, valid_rect.mTop )

		resizing_view = getParent()
		if resizing_view then
			-- Resize the parent
			LLRect orig_rect = resizing_view:getRect()
			LLRect scaled_rect = orig_rect
			S32 delta_x = screen_x - mDragLastScreenX
			S32 delta_y = screen_y - mDragLastScreenY
			mouse_dir=Vector2()
			-- use hysteresis on mouse motion to preserve user intent when mouse stops moving
			mouse_dir.X = (screen_x == mLastMouseScreenX) ? mLastMouseDir.X : screen_x - mLastMouseScreenX
			mouse_dir.Y = (screen_y == mLastMouseScreenY) ? mLastMouseDir.Y : screen_y - mLastMouseScreenY
			mLastMouseScreenX = screen_x
			mLastMouseScreenY = screen_y
			mLastMouseDir = mouse_dir

			S32 x_multiple = 1
			S32 y_multiple = 1
			
			if self:mCorner == LEFT_TOP then
				x_multiple = -1 
				y_multiple =  1
			elseif self:mCorner == LEFT_BOTTOM then
				x_multiple = -1 
				y_multiple = -1
			elseif self:mCorner == RIGHT_TOP then
				x_multiple =  1 
				y_multiple =  1
			elseif self:mCorner == RIGHT_BOTTOM then
				x_multiple =  1 
				y_multiple = -1	
			end

			S32 new_width = orig_rect.getWidth() + x_multiple * delta_x
			if new_width < mMinWidth then
				new_width = mMinWidth
				delta_x = x_multiple * (mMinWidth - orig_rect.getWidth())
			end

			S32 new_height = orig_rect.getHeight() + y_multiple * delta_y
			if new_height < mMinHeight then
				new_height = mMinHeight
				delta_y = y_multiple * (mMinHeight - orig_rect.getHeight())
			end

			if self:mCorner == LEFT_TOP then		
				scaled_rect.translate(delta_x, 0)			
			elseif self:mCorner == LEFT_BOTTOM then
				scaled_rect.translate(delta_x, delta_y)	
			elseif self:mCorner == RIGHT_TOP or self:mCorner == RIGHT_BOTTOM:	
				scaled_rect.translate(0, delta_y)			
			end

			-- temporarily set new parent rect
			scaled_rect.mRight = scaled_rect.mLeft + new_width
			scaled_rect.mTop = scaled_rect.mBottom + new_height
			resizing_view:setRect(scaled_rect)

			LLView* snap_view = NULL
			LLView* test_view = NULL

			-- now do snapping
			if self:mCorner == LEFT_TOP then
				snap_view = resizing_view:findSnapEdge(scaled_rect.mLeft, mouse_dir, SNAP_LEFT, SNAP_PARENT_AND_SIBLINGS, LLUI::sConfigGroup:getS32("SnapMargin"))
				test_view = resizing_view:findSnapEdge(scaled_rect.mTop, mouse_dir, SNAP_TOP, SNAP_PARENT_AND_SIBLINGS, LLUI::sConfigGroup:getS32("SnapMargin"))
				if (!snap_view) then
					snap_view = test_view
				end
			if self:mCorner == LEFT_BOTTOM then
				snap_view = resizing_view:findSnapEdge(scaled_rect.mLeft, mouse_dir, SNAP_LEFT, SNAP_PARENT_AND_SIBLINGS, LLUI::sConfigGroup:getS32("SnapMargin"))
				test_view = resizing_view:findSnapEdge(scaled_rect.mBottom, mouse_dir, SNAP_BOTTOM, SNAP_PARENT_AND_SIBLINGS, LLUI::sConfigGroup:getS32("SnapMargin"))
				if (!snap_view) then
					snap_view = test_view
				end
			if self:mCorner == RIGHT_TOP then
				snap_view = resizing_view:findSnapEdge(scaled_rect.mRight, mouse_dir, SNAP_RIGHT, SNAP_PARENT_AND_SIBLINGS, LLUI::sConfigGroup:getS32("SnapMargin"))
				test_view = resizing_view:findSnapEdge(scaled_rect.mTop, mouse_dir, SNAP_TOP, SNAP_PARENT_AND_SIBLINGS, LLUI::sConfigGroup:getS32("SnapMargin"))
				if (!snap_view) then
					snap_view = test_view
				end
			if self:mCorner == RIGHT_BOTTOM then	
				snap_view = resizing_view:findSnapEdge(scaled_rect.mRight, mouse_dir, SNAP_RIGHT, SNAP_PARENT_AND_SIBLINGS, LLUI::sConfigGroup:getS32("SnapMargin"))
				test_view = resizing_view:findSnapEdge(scaled_rect.mBottom, mouse_dir, SNAP_BOTTOM, SNAP_PARENT_AND_SIBLINGS, LLUI::sConfigGroup:getS32("SnapMargin"))
				if (!snap_view) then
					snap_view = test_view
				end
			end

			-- register "snap" behavior with snapped view
			resizing_view:snappedTo(snap_view)

			-- reset parent rect
			resizing_view:setRect(orig_rect)

			-- translate and scale to new shape
			resizing_view:userSetShape(scaled_rect)
			
			-- update last valid mouse cursor position based on resized view's actual size
			LLRect new_rect = resizing_view:getRect()
			if self:mCorner == LEFT_TOP then
				mDragLastScreenX += new_rect.mLeft - orig_rect.mLeft
				mDragLastScreenY += new_rect.mTop - orig_rect.mTop
			elseif self:mCorner == LEFT_BOTTOM then
				mDragLastScreenX += new_rect.mLeft - orig_rect.mLeft
				mDragLastScreenY += new_rect.mBottom- orig_rect.mBottom
			elseif self:mCorner == RIGHT_TOP then
				mDragLastScreenX += new_rect.mRight - orig_rect.mRight
				mDragLastScreenY += new_rect.mTop - orig_rect.mTop
			elseif self:mCorner == RIGHT_BOTTOM then
				mDragLastScreenX += new_rect.mRight - orig_rect.mRight
				mDragLastScreenY += new_rect.mBottom- orig_rect.mBottom
			end
		end

		handled = TRUE
	else -- don't have mouse capture
		if self:pointInHandle( x, y ) end
			handled = TRUE
		end
	end

	if handled then
		if self:mCorner == RIGHT_BOTTOM or self:mCorner == LEFT_TOP then	
			getWindow():setCursor(UI_CURSOR_SIZENWSE)
		elseif self:mCorner == LEFT_BOTTOM or self:mCorner == RIGHT_TOP then
			getWindow():setCursor(UI_CURSOR_SIZENESW)
		end
	end

	return handled
end -- end handleHover


-- assumes GL state is set for 2D
void LLResizeHandle::draw()
then
	if mImage.notNull() && getVisible() && (RIGHT_BOTTOM == self:mCorner) ) 
	then
		mImage:draw(0, 0)
	end
end


BOOL LLResizeHandle::pointInHandle( S32 x, S32 y )
then
	if pointInView(x, y) then
		TOP_BORDER = (getRect().getHeight() - RESIZE_BORDER_WIDTH)
		RIGHT_BORDER = (getRect().getWidth() - RESIZE_BORDER_WIDTH)

		if self:mCorner == LEFT_TOP then		
			return (x <= RESIZE_BORDER_WIDTH) or (y >= TOP_BORDER)
		elseif self:mCorner == LEFT_BOTTOM then	
			return (x <= RESIZE_BORDER_WIDTH) or (y <= RESIZE_BORDER_WIDTH)
		elseif self:mCorner == RIGHT_TOP then		
			return (x >= RIGHT_BORDER) or (y >= TOP_BORDER)
		elseif self:mCorner == RIGHT_BOTTOM then	
			return true
		end
	end
	return FALSE
end
