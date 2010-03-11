/* -----------------------------------------------------------------------------
 * See the LICENSE file for information on copyright, usage and redistribution
 * of SWIG, and the README file for authors - http://www.swig.org/release.html.
 *
 * LLUUID 
 *
 * LLUUID typemaps for LUA
 * ----------------------------------------------------------------------------- */

%rename(UUID)	LLUUID;
%naturalvar LLUUID;
%typemap(in,checkfn="lua_isstring") LLUUID
%{
	SWIG_contract_assert($1.set(lua_tostring(L,$input),false),"Must be of UUID format.");
%}
%typemap(out) LLUUID
%{ 
	lua_pushlstring(L,$1.asString().data(),$1.asString().size()); SWIG_arg++;
%}

%typemap(in,checkfn="lua_isstring")	const LLUUID& (LLUUID temp)
%{SWIG_contract_assert(temp.set(lua_tostring(L,$input),false),"Must be of UUID format.")
$1=&temp;%}

%typemap(out) const LLUUID&
%{ lua_pushlstring(L,$1->asString().data(),$1->asString().size()); SWIG_arg++;%}

%typemap(throws) LLUUID,LLUUID&,const LLUUID&
%{ lua_pushlstring(L,$1.asString().data(),$1.asString().size()); SWIG_fail;%}
%typemap(throws) LLUUID*,LLUUID*
%{ lua_pushlstring(L,$1->asString().data(),$1->asString().size()); SWIG_fail;%}

// and the typechecks
%typecheck(SWIG_TYPECHECK_STRING) LLUUID,const LLUUID& {
  $1 = lua_isstring(L,$input);
}

%typemap(in) LLUUID &INPUT=const LLUUID &;
%typemap(in, numinputs=0) LLUUID &OUTPUT (LLUUID temp)
%{ $1 = &temp; %}
%typemap(argout) LLUUID &OUTPUT
%{ lua_pushlstring(L,$1->asString().data(),$1->asString().size()); SWIG_arg++;%}
%typemap(in) LLUUID &INOUT =const LLUUID &;
%typemap(argout) LLUUID &INOUT = LLUUID &OUTPUT;
#define	   UUID_null	"00000000-0000-0000-0000-000000000000"

    class LLUUID {
      public:
        LLUUID ();
        explicit LLUUID(const char *in_string); // Convert from string.
        LLUUID &operator=(const LLUUID &rhs);
        std::string asString() const;
        BOOL set(const std::string& str, BOOL emit = TRUE);
        BOOL isNull() const;
        BOOL notNull() const;
        void setNull();
        static BOOL validate(const std::string& in_string);
    };
