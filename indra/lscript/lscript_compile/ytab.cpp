/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INTEGER = 258,
     FLOAT_TYPE = 259,
     STRING = 260,
     LLKEY = 261,
     VECTOR = 262,
     QUATERNION = 263,
     LIST = 264,
     STATE = 265,
     EVENT = 266,
     JUMP = 267,
     RETURN = 268,
     STATE_ENTRY = 269,
     STATE_EXIT = 270,
     TOUCH_START = 271,
     TOUCH = 272,
     TOUCH_END = 273,
     COLLISION_START = 274,
     COLLISION = 275,
     COLLISION_END = 276,
     LAND_COLLISION_START = 277,
     LAND_COLLISION = 278,
     LAND_COLLISION_END = 279,
     TIMER = 280,
     CHAT = 281,
     SENSOR = 282,
     NO_SENSOR = 283,
     CONTROL = 284,
     AT_TARGET = 285,
     NOT_AT_TARGET = 286,
     AT_ROT_TARGET = 287,
     NOT_AT_ROT_TARGET = 288,
     MONEY = 289,
     EMAIL = 290,
     RUN_TIME_PERMISSIONS = 291,
     INVENTORY = 292,
     ATTACH = 293,
     DATASERVER = 294,
     MOVING_START = 295,
     MOVING_END = 296,
     REZ = 297,
     OBJECT_REZ = 298,
     LINK_MESSAGE = 299,
     REMOTE_DATA = 300,
     HTTP_RESPONSE = 301,
     IDENTIFIER = 302,
     STATE_DEFAULT = 303,
     INTEGER_CONSTANT = 304,
     INTEGER_TRUE = 305,
     INTEGER_FALSE = 306,
     FP_CONSTANT = 307,
     STRING_CONSTANT = 308,
     INC_OP = 309,
     DEC_OP = 310,
     ADD_ASSIGN = 311,
     SUB_ASSIGN = 312,
     MUL_ASSIGN = 313,
     DIV_ASSIGN = 314,
     MOD_ASSIGN = 315,
     EQ = 316,
     NEQ = 317,
     GEQ = 318,
     LEQ = 319,
     BOOLEAN_AND = 320,
     BOOLEAN_OR = 321,
     SHIFT_LEFT = 322,
     SHIFT_RIGHT = 323,
     IF = 324,
     ELSE = 325,
     FOR = 326,
     DO = 327,
     WHILE = 328,
     PRINT = 329,
     PERIOD = 330,
     ZERO_VECTOR = 331,
     ZERO_ROTATION = 332,
     LOWER_THAN_ELSE = 333,
     INITIALIZER = 334
   };
#endif
/* Tokens.  */
#define INTEGER 258
#define FLOAT_TYPE 259
#define STRING 260
#define LLKEY 261
#define VECTOR 262
#define QUATERNION 263
#define LIST 264
#define STATE 265
#define EVENT 266
#define JUMP 267
#define RETURN 268
#define STATE_ENTRY 269
#define STATE_EXIT 270
#define TOUCH_START 271
#define TOUCH 272
#define TOUCH_END 273
#define COLLISION_START 274
#define COLLISION 275
#define COLLISION_END 276
#define LAND_COLLISION_START 277
#define LAND_COLLISION 278
#define LAND_COLLISION_END 279
#define TIMER 280
#define CHAT 281
#define SENSOR 282
#define NO_SENSOR 283
#define CONTROL 284
#define AT_TARGET 285
#define NOT_AT_TARGET 286
#define AT_ROT_TARGET 287
#define NOT_AT_ROT_TARGET 288
#define MONEY 289
#define EMAIL 290
#define RUN_TIME_PERMISSIONS 291
#define INVENTORY 292
#define ATTACH 293
#define DATASERVER 294
#define MOVING_START 295
#define MOVING_END 296
#define REZ 297
#define OBJECT_REZ 298
#define LINK_MESSAGE 299
#define REMOTE_DATA 300
#define HTTP_RESPONSE 301
#define IDENTIFIER 302
#define STATE_DEFAULT 303
#define INTEGER_CONSTANT 304
#define INTEGER_TRUE 305
#define INTEGER_FALSE 306
#define FP_CONSTANT 307
#define STRING_CONSTANT 308
#define INC_OP 309
#define DEC_OP 310
#define ADD_ASSIGN 311
#define SUB_ASSIGN 312
#define MUL_ASSIGN 313
#define DIV_ASSIGN 314
#define MOD_ASSIGN 315
#define EQ 316
#define NEQ 317
#define GEQ 318
#define LEQ 319
#define BOOLEAN_AND 320
#define BOOLEAN_OR 321
#define SHIFT_LEFT 322
#define SHIFT_RIGHT 323
#define IF 324
#define ELSE 325
#define FOR 326
#define DO 327
#define WHILE 328
#define PRINT 329
#define PERIOD 330
#define ZERO_VECTOR 331
#define ZERO_ROTATION 332
#define LOWER_THAN_ELSE 333
#define INITIALIZER 334




/* Copy the first part of user declarations.  */
#line 1 "indra.y"

	#include "linden_common.h"
	#include "lscript_tree.h"

    #ifdef __cplusplus
    extern "C" {
    #endif

	int yylex(void);
	int yyparse( void );
	int yyerror(const char *fmt, ...);

    #if LL_LINUX
    // broken yacc codegen...  --ryan.
    #define getenv getenv_workaround
    #endif

    #ifdef LL_WINDOWS
	#pragma warning (disable : 4702) // warning C4702: unreachable code
	#pragma warning( disable : 4065 )	// warning: switch statement contains 'default' but no 'case' labels
	#endif

    #ifdef __cplusplus
    }
    #endif


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 29 "indra.y"
{
	S32								ival;
	F32								fval;
	char							*sval;
	class LLScriptType				*type;
	class LLScriptConstant			*constant;
	class LLScriptIdentifier		*identifier;
	class LLScriptSimpleAssignable	*assignable;
	class LLScriptGlobalVariable	*global;
	class LLScriptEvent				*event;
	class LLScriptEventHandler		*handler;
	class LLScriptExpression		*expression;
	class LLScriptStatement			*statement;
	class LLScriptGlobalFunctions	*global_funcs;
	class LLScriptFunctionDec		*global_decl;
	class LLScriptState				*state;
	class LLScritpGlobalStorage		*global_store;
	class LLScriptScript			*script;
}
/* Line 187 of yacc.c.  */
#line 301 "ytab.cpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 314 "ytab.cpp"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  21
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1354

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  102
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  78
/* YYNRULES -- Number of rules.  */
#define YYNRULES  209
/* YYNRULES -- Number of states.  */
#define YYNSTATES  565

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   334

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    82,     2,     2,     2,    90,    91,     2,
      94,    95,    88,    87,    97,    81,     2,    89,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    79,
      85,    84,    86,     2,    80,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    98,     2,    99,    93,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   100,    92,   101,    83,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    96
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     6,     8,    10,    13,    15,    17,    20,
      23,    28,    30,    32,    34,    36,    38,    40,    42,    44,
      46,    49,    51,    53,    55,    58,    60,    62,    70,    72,
      82,    84,    88,    91,    93,    97,    99,   101,   103,   105,
     107,   109,   111,   113,   118,   123,   129,   135,   137,   141,
     144,   146,   149,   151,   154,   159,   165,   167,   170,   173,
     176,   179,   182,   185,   188,   191,   194,   197,   200,   203,
     206,   209,   212,   215,   218,   221,   224,   227,   230,   233,
     236,   239,   242,   245,   248,   251,   254,   257,   260,   263,
     266,   269,   273,   277,   283,   289,   295,   301,   307,   313,
     319,   325,   331,   343,   347,   359,   363,   372,   390,   396,
     402,   408,   417,   421,   425,   429,   444,   450,   454,   466,
     472,   478,   493,   514,   529,   532,   536,   538,   541,   543,
     547,   551,   555,   559,   563,   566,   569,   572,   574,   580,
     588,   598,   606,   612,   615,   620,   621,   623,   625,   629,
     630,   632,   634,   638,   639,   641,   643,   647,   649,   653,
     657,   661,   665,   669,   673,   677,   681,   685,   689,   693,
     697,   701,   705,   709,   713,   717,   721,   725,   729,   733,
     737,   741,   745,   748,   751,   754,   757,   760,   762,   764,
     768,   773,   778,   783,   790,   792,   794,   796,   798,   801,
     804,   809,   814,   816,   824,   826,   836,   838,   842,   844
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     103,     0,    -1,   104,   123,    -1,   123,    -1,   105,    -1,
     105,   104,    -1,   107,    -1,   120,    -1,   119,    47,    -1,
     106,    79,    -1,   106,    84,   108,    79,    -1,   109,    -1,
     116,    -1,    47,    -1,   110,    -1,   113,    -1,   112,    -1,
     111,    -1,    53,    -1,    52,    -1,    81,    52,    -1,    49,
      -1,    50,    -1,    51,    -1,    81,    49,    -1,   114,    -1,
     115,    -1,    85,   108,    97,   108,    97,   108,    86,    -1,
      76,    -1,    85,   108,    97,   108,    97,   108,    97,   108,
      86,    -1,    77,    -1,    98,   117,    99,    -1,    98,    99,
      -1,   118,    -1,   118,    97,   117,    -1,   109,    -1,     3,
      -1,     4,    -1,     5,    -1,     6,    -1,     7,    -1,     8,
      -1,     9,    -1,    47,    94,    95,   162,    -1,   106,    94,
      95,   162,    -1,    47,    94,   121,    95,   162,    -1,   106,
      94,   121,    95,   162,    -1,   122,    -1,   122,    97,   121,
      -1,   119,    47,    -1,   125,    -1,   125,   124,    -1,   126,
      -1,   126,   124,    -1,    48,   100,   127,   101,    -1,    10,
      47,   100,   127,   101,    -1,   128,    -1,   128,   127,    -1,
     129,   162,    -1,   130,   162,    -1,   131,   162,    -1,   132,
     162,    -1,   133,   162,    -1,   134,   162,    -1,   135,   162,
      -1,   136,   162,    -1,   137,   162,    -1,   138,   162,    -1,
     139,   162,    -1,   152,   162,    -1,   153,   162,    -1,   154,
     162,    -1,   155,   162,    -1,   140,   162,    -1,   141,   162,
      -1,   142,   162,    -1,   143,   162,    -1,   144,   162,    -1,
     145,   162,    -1,   146,   162,    -1,   147,   162,    -1,   148,
     162,    -1,   149,   162,    -1,   156,   162,    -1,   150,   162,
      -1,   151,   162,    -1,   157,   162,    -1,   158,   162,    -1,
     159,   162,    -1,   160,   162,    -1,   161,   162,    -1,    14,
      94,    95,    -1,    15,    94,    95,    -1,    16,    94,     3,
      47,    95,    -1,    17,    94,     3,    47,    95,    -1,    18,
      94,     3,    47,    95,    -1,    19,    94,     3,    47,    95,
      -1,    20,    94,     3,    47,    95,    -1,    21,    94,     3,
      47,    95,    -1,    22,    94,     7,    47,    95,    -1,    23,
      94,     7,    47,    95,    -1,    24,    94,     7,    47,    95,
      -1,    30,    94,     3,    47,    97,     7,    47,    97,     7,
      47,    95,    -1,    31,    94,    95,    -1,    32,    94,     3,
      47,    97,     8,    47,    97,     8,    47,    95,    -1,    33,
      94,    95,    -1,    34,    94,     6,    47,    97,     3,    47,
      95,    -1,    35,    94,     5,    47,    97,     5,    47,    97,
       5,    47,    97,     5,    47,    97,     3,    47,    95,    -1,
      36,    94,     3,    47,    95,    -1,    37,    94,     3,    47,
      95,    -1,    38,    94,     6,    47,    95,    -1,    39,    94,
       6,    47,    97,     5,    47,    95,    -1,    40,    94,    95,
      -1,    41,    94,    95,    -1,    25,    94,    95,    -1,    26,
      94,     3,    47,    97,     5,    47,    97,     6,    47,    97,
       5,    47,    95,    -1,    27,    94,     3,    47,    95,    -1,
      28,    94,    95,    -1,    29,    94,     6,    47,    97,     3,
      47,    97,     3,    47,    95,    -1,    42,    94,     3,    47,
      95,    -1,    43,    94,     6,    47,    95,    -1,    44,    94,
       3,    47,    97,     3,    47,    97,     5,    47,    97,     6,
      47,    95,    -1,    45,    94,     3,    47,    97,     6,    47,
      97,     6,    47,    97,     5,    47,    97,     3,    47,    97,
       5,    47,    95,    -1,    46,    94,     6,    47,    97,     3,
      47,    97,     9,    47,    97,     5,    47,    95,    -1,   100,
     101,    -1,   100,   163,   101,    -1,   164,    -1,   163,   164,
      -1,    79,    -1,    10,    47,    79,    -1,    10,    48,    79,
      -1,    12,    47,    79,    -1,    80,    47,    79,    -1,    13,
     172,    79,    -1,    13,    79,    -1,   172,    79,    -1,   165,
      79,    -1,   162,    -1,    69,    94,   172,    95,   164,    -1,
      69,    94,   172,    95,   164,    70,   164,    -1,    71,    94,
     166,    79,   172,    79,   166,    95,   164,    -1,    72,   164,
      73,    94,   172,    95,    79,    -1,    73,    94,   172,    95,
     164,    -1,   119,    47,    -1,   119,    47,    84,   172,    -1,
      -1,   167,    -1,   172,    -1,   172,    97,   167,    -1,    -1,
     169,    -1,   172,    -1,   172,    97,   169,    -1,    -1,   171,
      -1,   172,    -1,   172,    97,   171,    -1,   173,    -1,   179,
      84,   172,    -1,   179,    56,   172,    -1,   179,    57,   172,
      -1,   179,    58,   172,    -1,   179,    59,   172,    -1,   179,
      60,   172,    -1,   172,    61,   172,    -1,   172,    62,   172,
      -1,   172,    64,   172,    -1,   172,    63,   172,    -1,   172,
      85,   172,    -1,   172,    86,   172,    -1,   172,    87,   172,
      -1,   172,    81,   172,    -1,   172,    88,   172,    -1,   172,
      89,   172,    -1,   172,    90,   172,    -1,   172,    91,   172,
      -1,   172,    92,   172,    -1,   172,    93,   172,    -1,   172,
      65,   172,    -1,   172,    66,   172,    -1,   172,    67,   172,
      -1,   172,    68,   172,    -1,    81,   172,    -1,    82,   172,
      -1,    83,   172,    -1,    54,   179,    -1,    55,   179,    -1,
     174,    -1,   175,    -1,    94,   172,    95,    -1,    94,   119,
      95,   179,    -1,    94,   119,    95,   110,    -1,    94,   119,
      95,   175,    -1,    94,   119,    95,    94,   172,    95,    -1,
     176,    -1,   177,    -1,   178,    -1,   179,    -1,   179,    54,
      -1,   179,    55,    -1,    47,    94,   168,    95,    -1,    74,
      94,   172,    95,    -1,   110,    -1,    85,   172,    97,   172,
      97,   172,    86,    -1,    76,    -1,    85,   172,    97,   172,
      97,   172,    97,   172,    86,    -1,    77,    -1,    98,   170,
      99,    -1,    47,    -1,    47,    75,    47,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   270,   270,   276,   285,   289,   297,   302,   310,   318,
     323,   331,   335,   342,   349,   354,   361,   366,   371,   379,
     383,   390,   394,   398,   402,   409,   413,   420,   425,   445,
     450,   474,   479,   487,   491,   499,   506,   511,   516,   521,
     526,   531,   536,   544,   551,   556,   563,   571,   575,   583,
     593,   597,   605,   609,   617,   627,   637,   641,   649,   654,
     659,   664,   669,   674,   679,   684,   689,   694,   699,   704,
     709,   714,   719,   724,   729,   734,   739,   744,   749,   754,
     759,   764,   769,   774,   779,   784,   789,   794,   799,   804,
     809,   817,   825,   833,   843,   853,   863,   873,   883,   893,
     903,   913,   923,   937,   945,   959,   967,   979,   997,  1007,
    1017,  1027,  1039,  1047,  1055,  1063,  1079,  1089,  1097,  1111,
    1121,  1131,  1147,  1167,  1183,  1188,  1196,  1200,  1208,  1213,
    1220,  1227,  1234,  1241,  1246,  1251,  1256,  1260,  1264,  1270,
    1277,  1283,  1289,  1298,  1305,  1316,  1319,  1326,  1331,  1340,
    1343,  1350,  1355,  1364,  1367,  1374,  1379,  1387,  1391,  1396,
    1401,  1406,  1411,  1416,  1421,  1426,  1431,  1436,  1441,  1446,
    1451,  1456,  1461,  1466,  1471,  1476,  1481,  1486,  1491,  1496,
    1501,  1506,  1514,  1519,  1524,  1529,  1534,  1539,  1543,  1547,
    1555,  1560,  1567,  1572,  1580,  1584,  1588,  1592,  1596,  1601,
    1606,  1613,  1618,  1626,  1631,  1651,  1656,  1680,  1688,  1695
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "INTEGER", "FLOAT_TYPE", "STRING",
  "LLKEY", "VECTOR", "QUATERNION", "LIST", "STATE", "EVENT", "JUMP",
  "RETURN", "STATE_ENTRY", "STATE_EXIT", "TOUCH_START", "TOUCH",
  "TOUCH_END", "COLLISION_START", "COLLISION", "COLLISION_END",
  "LAND_COLLISION_START", "LAND_COLLISION", "LAND_COLLISION_END", "TIMER",
  "CHAT", "SENSOR", "NO_SENSOR", "CONTROL", "AT_TARGET", "NOT_AT_TARGET",
  "AT_ROT_TARGET", "NOT_AT_ROT_TARGET", "MONEY", "EMAIL",
  "RUN_TIME_PERMISSIONS", "INVENTORY", "ATTACH", "DATASERVER",
  "MOVING_START", "MOVING_END", "REZ", "OBJECT_REZ", "LINK_MESSAGE",
  "REMOTE_DATA", "HTTP_RESPONSE", "IDENTIFIER", "STATE_DEFAULT",
  "INTEGER_CONSTANT", "INTEGER_TRUE", "INTEGER_FALSE", "FP_CONSTANT",
  "STRING_CONSTANT", "INC_OP", "DEC_OP", "ADD_ASSIGN", "SUB_ASSIGN",
  "MUL_ASSIGN", "DIV_ASSIGN", "MOD_ASSIGN", "EQ", "NEQ", "GEQ", "LEQ",
  "BOOLEAN_AND", "BOOLEAN_OR", "SHIFT_LEFT", "SHIFT_RIGHT", "IF", "ELSE",
  "FOR", "DO", "WHILE", "PRINT", "PERIOD", "ZERO_VECTOR", "ZERO_ROTATION",
  "LOWER_THAN_ELSE", "';'", "'@'", "'-'", "'!'", "'~'", "'='", "'<'",
  "'>'", "'+'", "'*'", "'/'", "'%'", "'&'", "'|'", "'^'", "'('", "')'",
  "INITIALIZER", "','", "'['", "']'", "'{'", "'}'", "$accept",
  "lscript_program", "globals", "global", "name_type", "global_variable",
  "simple_assignable", "simple_assignable_no_list", "constant",
  "fp_constant", "integer_constant", "special_constant", "vector_constant",
  "quaternion_constant", "list_constant", "list_entries", "list_entry",
  "typename", "global_function", "function_parameters",
  "function_parameter", "states", "other_states", "default", "state",
  "state_body", "event", "state_entry", "state_exit", "touch_start",
  "touch", "touch_end", "collision_start", "collision", "collision_end",
  "land_collision_start", "land_collision", "land_collision_end",
  "at_target", "not_at_target", "at_rot_target", "not_at_rot_target",
  "money", "email", "run_time_permissions", "inventory", "attach",
  "dataserver", "moving_start", "moving_end", "timer", "chat", "sensor",
  "no_sensor", "control", "rez", "object_rez", "link_message",
  "remote_data", "http_response", "compound_statement", "statements",
  "statement", "declaration", "forexpressionlist", "nextforexpressionlist",
  "funcexpressionlist", "nextfuncexpressionlist", "listexpressionlist",
  "nextlistexpressionlist", "expression", "unaryexpression", "typecast",
  "unarypostfixexpression", "vector_initializer", "quaternion_initializer",
  "list_initializer", "lvalue", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,    59,
      64,    45,    33,   126,    61,    60,    62,    43,    42,    47,
      37,    38,   124,    94,    40,    41,   334,    44,    91,    93,
     123,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   102,   103,   103,   104,   104,   105,   105,   106,   107,
     107,   108,   108,   109,   109,   109,   110,   110,   110,   111,
     111,   112,   112,   112,   112,   113,   113,   114,   114,   115,
     115,   116,   116,   117,   117,   118,   119,   119,   119,   119,
     119,   119,   119,   120,   120,   120,   120,   121,   121,   122,
     123,   123,   124,   124,   125,   126,   127,   127,   128,   128,
     128,   128,   128,   128,   128,   128,   128,   128,   128,   128,
     128,   128,   128,   128,   128,   128,   128,   128,   128,   128,
     128,   128,   128,   128,   128,   128,   128,   128,   128,   128,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   152,   153,   154,   155,   156,   157,
     158,   159,   160,   161,   162,   162,   163,   163,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   164,   165,   165,   166,   166,   167,   167,   168,
     168,   169,   169,   170,   170,   171,   171,   172,   172,   172,
     172,   172,   172,   172,   172,   172,   172,   172,   172,   172,
     172,   172,   172,   172,   172,   172,   172,   172,   172,   172,
     172,   172,   173,   173,   173,   173,   173,   173,   173,   173,
     174,   174,   174,   174,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   176,   176,   177,   177,   178,   179,   179
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     1,     1,     2,     1,     1,     2,     2,
       4,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     1,     1,     1,     2,     1,     1,     7,     1,     9,
       1,     3,     2,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     4,     4,     5,     5,     1,     3,     2,
       1,     2,     1,     2,     4,     5,     1,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     3,     3,     5,     5,     5,     5,     5,     5,     5,
       5,     5,    11,     3,    11,     3,     8,    17,     5,     5,
       5,     8,     3,     3,     3,    14,     5,     3,    11,     5,
       5,    14,    20,    14,     2,     3,     1,     2,     1,     3,
       3,     3,     3,     3,     2,     2,     2,     1,     5,     7,
       9,     7,     5,     2,     4,     0,     1,     1,     3,     0,
       1,     1,     3,     0,     1,     1,     3,     1,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     2,     2,     2,     2,     2,     1,     1,     3,
       4,     4,     4,     6,     1,     1,     1,     1,     2,     2,
       4,     4,     1,     7,     1,     9,     1,     3,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,    36,    37,    38,    39,    40,    41,    42,     0,     0,
       0,     0,     4,     0,     6,     0,     7,     3,    50,     0,
       0,     1,     2,     5,     9,     0,     0,     8,     0,    51,
      52,     0,     0,     0,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    56,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    13,    21,    22,    23,    19,    18,    28,
      30,     0,     0,     0,     0,    11,    14,    17,    16,    15,
      25,    26,    12,     0,     0,     0,    53,     0,    43,    49,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    54,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    84,    85,
      69,    70,    71,    72,    83,    86,    87,    88,    89,    90,
      24,    20,     0,    32,    35,     0,    33,    10,    44,     0,
       0,     0,     0,     0,   208,     0,     0,     0,     0,     0,
       0,     0,   204,   206,   128,     0,     0,     0,     0,     0,
       0,   153,   124,   202,     0,   137,     0,   126,     0,     0,
     157,   187,   188,   194,   195,   196,   197,    45,    48,    91,
      92,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     114,     0,     0,   117,     0,     0,   103,     0,   105,     0,
       0,     0,     0,     0,     0,   112,   113,     0,     0,     0,
       0,     0,     0,    31,     0,    46,     0,     0,     0,     0,
     134,     0,     0,   149,   208,   185,   186,     0,   145,     0,
       0,     0,     0,    21,    19,   182,   183,   184,     0,     0,
       0,     0,   154,   155,   143,   125,   127,   136,     0,     0,
       0,     0,     0,     0,     0,     0,   135,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   198,   199,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    34,    55,   129,   130,   131,   133,   209,     0,   150,
     151,     0,     0,   146,   147,     0,     0,     0,   132,     0,
       0,   189,   207,     0,     0,   164,   165,   167,   166,   178,
     179,   180,   181,   171,   168,   169,   170,   172,   173,   174,
     175,   176,   177,   159,   160,   161,   162,   163,   158,    93,
      94,    95,    96,    97,    98,    99,   100,   101,     0,   116,
       0,     0,     0,     0,     0,   108,   109,   110,     0,   119,
     120,     0,     0,     0,     0,   200,     0,     0,     0,     0,
       0,     0,   201,     0,     0,   191,   192,   190,   156,   144,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   152,   138,     0,   148,     0,   142,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    27,
       0,     0,   145,     0,     0,   193,     0,     0,     0,     0,
     106,     0,   111,     0,     0,     0,     0,   139,     0,   141,
     203,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      29,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   140,   205,     0,   118,   102,   104,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   115,     0,   121,     0,   123,     0,     0,     0,     0,
     107,     0,     0,     0,   122
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    10,    11,    12,    13,    14,   114,   115,   233,   117,
     118,   119,   120,   121,   122,   205,   206,   234,    16,    33,
      34,    17,    29,    18,    30,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     235,   236,   237,   238,   382,   383,   378,   379,   311,   312,
     239,   240,   241,   242,   243,   244,   245,   246
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -213
static const yytype_int16 yypact[] =
{
     143,  -213,  -213,  -213,  -213,  -213,  -213,  -213,   -88,   -90,
      16,   -22,   206,   -67,  -213,   -16,  -213,  -213,    28,    71,
    1046,  -213,  -213,  -213,  -213,   459,    99,  -213,    39,  -213,
      28,     9,    82,    23,    33,    43,    48,    49,    59,    68,
      73,    74,    75,    76,    78,    79,    81,    83,    84,    85,
      88,    90,    93,    94,    95,    98,   101,   102,   103,   105,
     113,   114,   122,   123,   124,   125,   127,   132,    64,  1046,
       9,     9,     9,     9,     9,     9,     9,     9,     9,     9,
       9,     9,     9,     9,     9,     9,     9,     9,     9,     9,
       9,     9,     9,     9,     9,     9,     9,     9,     9,     9,
       9,     9,     9,  -213,  -213,  -213,  -213,  -213,  -213,  -213,
    -213,    32,   459,   211,    97,  -213,  -213,  -213,  -213,  -213,
    -213,  -213,  -213,     9,   134,   137,  -213,   151,  -213,  -213,
       9,   322,   149,   152,   190,   245,   247,   251,   252,   253,
     258,   259,   260,   173,   267,   279,   188,   280,   282,   194,
     287,   196,   288,   290,   294,   295,   293,   298,   198,   205,
     302,   300,   304,   305,   306,  -213,  -213,  -213,  -213,  -213,
    -213,  -213,  -213,  -213,  -213,  -213,  -213,  -213,  -213,  -213,
    -213,  -213,  -213,  -213,  -213,  -213,  -213,  -213,  -213,  -213,
    -213,  -213,  -213,  -213,  -213,  -213,  -213,  -213,  -213,  -213,
    -213,  -213,   204,  -213,  -213,   212,   216,  -213,  -213,     9,
    1046,    35,   269,   419,   -64,   285,   285,   220,   240,   351,
     241,   242,  -213,  -213,  -213,   291,   469,   479,   479,   479,
     405,   479,  -213,  -213,   296,  -213,   268,  -213,   273,  1032,
    -213,  -213,  -213,  -213,  -213,  -213,   331,  -213,  -213,  -213,
    -213,   299,   320,   323,   324,   325,   326,   327,   328,   329,
    -213,   330,   333,  -213,   334,   335,  -213,   336,  -213,   337,
     345,   346,   347,   348,   349,  -213,  -213,   350,   352,   360,
     369,   370,   459,  -213,   539,  -213,   277,   339,   342,   356,
    -213,  1065,   379,   479,   354,  -213,  -213,   479,   479,   364,
     479,   479,   359,  -213,  -213,    51,  -213,  -213,   567,   344,
     789,   341,  -213,   604,   357,  -213,  -213,  -213,   479,   479,
     479,   479,   479,   479,   479,   479,  -213,   479,   479,   479,
     479,   479,   479,   479,   479,   479,   479,  -213,  -213,   479,
     479,   479,   479,   479,   479,   353,   355,   358,   366,   367,
     368,   372,   380,   381,   383,   382,   386,   387,   388,   392,
     394,   397,   399,   402,   408,   412,   420,   417,   428,   430,
     440,  -213,  -213,  -213,  -213,  -213,  -213,  -213,   443,  -213,
     641,   824,   363,  -213,   678,   384,   859,   894,  -213,   479,
     519,  -213,  -213,   479,   479,  1264,  1264,   -53,   -53,  1179,
    1179,    10,    10,    51,   -53,   -53,    51,  -213,  -213,  -213,
    1254,  1212,  1223,  1131,  1131,  1131,  1131,  1131,  1131,  -213,
    -213,  -213,  -213,  -213,  -213,  -213,  -213,  -213,   438,  -213,
     441,   439,   456,   444,   534,  -213,  -213,  -213,   536,  -213,
    -213,   544,   542,   546,   459,  -213,   479,   351,   479,   479,
     479,   351,  -213,   715,   479,  -213,  -213,    41,  -213,  1131,
     495,   511,   512,   518,   527,   528,   538,   540,   547,   550,
     -57,  -213,   531,  1098,  -213,   929,  -213,   479,   964,   501,
     502,   505,   510,   513,   517,   514,   521,   522,   524,  -213,
     459,   351,   479,   543,   752,  -213,   619,   607,   629,   618,
    -213,   632,  -213,   633,   634,   630,   555,  -213,   548,  -213,
     529,   479,   595,   597,   598,   599,   600,   602,   603,   614,
    -213,   351,  1164,   554,   568,   578,   579,   565,   580,   581,
     582,  -213,   529,   670,  -213,  -213,  -213,   671,   674,   676,
     677,   636,   637,   639,   640,   651,   593,   613,   605,   615,
     616,  -213,   696,  -213,   710,  -213,   667,   668,   621,   620,
    -213,   713,   672,   625,  -213
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -213,  -213,   709,  -213,  -213,  -213,  -111,  -110,   -25,  -213,
    -213,  -213,  -213,  -213,  -213,   451,  -213,    13,  -213,     7,
    -213,   712,   694,  -213,  -213,   -65,  -213,  -213,  -213,  -213,
    -213,  -213,  -213,  -213,  -213,  -213,  -213,  -213,  -213,  -213,
    -213,  -213,  -213,  -213,  -213,  -213,  -213,  -213,  -213,  -213,
    -213,  -213,  -213,  -213,  -213,  -213,  -213,  -213,  -213,  -213,
     -29,  -213,  -212,  -213,   233,   301,  -213,   303,  -213,   343,
    -208,  -213,  -213,   361,  -213,  -213,  -213,  -207
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint16 yytable[] =
{
     116,   202,   128,   204,   166,   291,    19,   299,   295,   296,
      20,   292,    24,    15,   324,   325,    21,    25,   305,   306,
     307,   308,   310,   313,   316,    15,     9,    26,   327,   489,
     293,    27,    32,   124,   330,   331,   332,   333,    28,    32,
     490,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   187,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,     1,     2,     3,     4,     5,     6,
       7,   200,   287,   288,   201,   380,   125,   116,   116,   381,
     384,   327,   386,   387,   208,   337,   338,   330,   331,   332,
     333,   247,     1,     2,     3,     4,     5,     6,     7,   127,
     395,   396,   397,   398,   399,   400,   401,   402,   130,   403,
     404,   405,   406,   407,   408,   409,   410,   411,   412,   129,
     131,   413,   414,   415,   416,   417,   418,   132,   248,   331,
     332,   333,   133,   134,    32,   286,     1,     2,     3,     4,
       5,     6,     7,   135,     1,     2,     3,     4,     5,     6,
       7,   211,   136,   212,   213,   165,    31,   137,   138,   139,
     140,   370,   141,   142,   204,   143,   207,   144,   145,   146,
     285,   453,   147,   457,   148,   313,   459,   149,   150,   151,
       8,     9,   152,   251,   123,   153,   154,   155,   214,   156,
     104,   105,   106,   107,   108,   215,   216,   157,   158,     1,
       2,     3,     4,     5,     6,     7,   159,   160,   161,   162,
     217,   163,   218,   219,   220,   221,   164,   222,   223,   209,
     224,   225,   226,   227,   228,   472,   229,   210,   380,   476,
     473,   384,   475,   309,   249,   230,   478,   250,   252,   231,
     253,   127,   232,     8,   254,   255,   256,   116,   103,   116,
     104,   105,   106,   107,   108,   257,   258,   259,   260,   494,
     261,     1,     2,     3,     4,     5,     6,     7,   211,   507,
     212,   213,   262,   263,   384,   265,   264,   109,   110,   266,
     267,   268,   111,   275,   269,   270,   112,   271,   272,   273,
     276,   282,   405,   522,   274,   277,   278,   279,   280,   531,
     203,   283,   281,   284,   297,   214,   289,   104,   105,   106,
     107,   108,   215,   216,   405,     1,     2,     3,     4,     5,
       6,     7,   294,   470,   298,   300,   301,   217,   302,   218,
     219,   220,   221,   314,   222,   223,   345,   224,   225,   226,
     227,   228,   317,   229,     1,     2,     3,     4,     5,     6,
       7,   211,   230,   212,   213,   455,   231,   346,   127,   315,
     347,   348,   349,   350,   351,   352,   353,   354,   372,   506,
     355,   356,   357,   358,   359,   337,   338,   339,   340,   341,
     342,   343,   360,   361,   362,   363,   364,   365,   214,   366,
     104,   105,   106,   107,   108,   215,   216,   367,     1,     2,
       3,     4,     5,     6,     7,   344,   368,   369,   373,   116,
     217,   374,   218,   219,   220,   221,   377,   222,   223,   292,
     224,   225,   226,   227,   228,   375,   229,   385,   388,   390,
     392,   394,   448,   460,   461,   230,   462,   464,   419,   231,
     420,   127,   214,   421,   104,   105,   106,   107,   108,   215,
     216,   422,   423,   424,   463,   116,   214,   425,   104,   105,
     106,   107,   108,   215,   216,   426,   427,   429,   450,   221,
     428,   222,   223,   430,   431,   432,   226,   227,   228,   433,
     229,   434,   435,   221,   436,   222,   223,   437,   290,   230,
     226,   227,   228,   231,   229,   438,   103,   439,   104,   105,
     106,   107,   108,   230,   441,   440,   214,   231,   303,   105,
     106,   304,   108,   215,   216,   442,   214,   443,   104,   105,
     106,   107,   108,   215,   216,   109,   110,   444,   445,   465,
     111,   466,   479,   221,   112,   222,   223,   467,   468,   469,
     226,   227,   228,   221,   229,   222,   223,   113,   480,   481,
     226,   227,   228,   230,   229,   482,   214,   231,   104,   105,
     106,   107,   108,   230,   483,   484,   214,   231,   104,   105,
     106,   107,   108,   215,   216,   485,   103,   486,   104,   105,
     106,   107,   108,   221,   487,   222,   223,   488,   496,   497,
     111,   491,   498,   221,   229,   222,   223,   499,   500,   502,
     513,   227,   228,   454,   501,   109,   110,   231,   503,   504,
     111,   505,   509,   230,   112,   512,   515,   231,   318,   319,
     320,   321,   322,   323,   324,   325,   514,   516,   517,   519,
     518,   520,   523,   521,   524,   525,   526,   527,   327,   528,
     529,   533,   328,   329,   330,   331,   332,   333,   334,   335,
     336,   530,   537,   534,   389,   318,   319,   320,   321,   322,
     323,   324,   325,   535,   536,   541,   542,   538,   539,   540,
     543,   544,   545,   546,   547,   327,   548,   549,   551,   328,
     329,   330,   331,   332,   333,   334,   335,   336,   550,   556,
     553,   393,   318,   319,   320,   321,   322,   323,   324,   325,
     552,   555,   554,   557,   558,   559,   560,   561,   562,   563,
     564,    23,   327,    22,   126,   508,   328,   329,   330,   331,
     332,   333,   334,   335,   336,   371,   458,     0,   446,   318,
     319,   320,   321,   322,   323,   324,   325,     0,     0,   471,
     474,   456,     0,     0,     0,     0,     0,     0,     0,   327,
       0,     0,     0,   328,   329,   330,   331,   332,   333,   334,
     335,   336,     0,     0,     0,   449,   318,   319,   320,   321,
     322,   323,   324,   325,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   327,     0,     0,     0,
     328,   329,   330,   331,   332,   333,   334,   335,   336,     0,
       0,     0,   477,   318,   319,   320,   321,   322,   323,   324,
     325,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   327,     0,     0,     0,   328,   510,   330,
     331,   332,   333,   334,   335,   336,     0,     0,     0,   511,
     318,   319,   320,   321,   322,   323,   324,   325,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     327,     0,     0,     0,   328,   329,   330,   331,   332,   333,
     334,   335,   336,     0,   391,   318,   319,   320,   321,   322,
     323,   324,   325,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   327,     0,     0,     0,   328,
     329,   330,   331,   332,   333,   334,   335,   336,     0,   447,
     318,   319,   320,   321,   322,   323,   324,   325,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     327,     0,     0,     0,   328,   329,   330,   331,   332,   333,
     334,   335,   336,     0,   451,   318,   319,   320,   321,   322,
     323,   324,   325,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   327,     0,     0,     0,   328,
     329,   330,   331,   332,   333,   334,   335,   336,     0,   452,
     318,   319,   320,   321,   322,   323,   324,   325,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     327,     0,     0,     0,   328,   329,   330,   331,   332,   333,
     334,   335,   336,     0,   493,   318,   319,   320,   321,   322,
     323,   324,   325,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   327,     0,     0,     0,   328,
     329,   330,   331,   332,   333,   334,   335,   336,     0,   495,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,   318,   319,   320,   321,   322,   323,   324,
     325,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   326,     0,   327,     0,     0,     0,   328,   329,   330,
     331,   332,   333,   334,   335,   336,   318,   319,   320,   321,
     322,   323,   324,   325,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   376,     0,   327,     0,     0,     0,
     328,   329,   330,   331,   332,   333,   334,   335,   336,   318,
     319,   320,   321,   322,   323,   324,   325,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   492,     0,   327,
       0,     0,     0,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   318,   319,   320,   321,   322,   323,   324,   325,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   327,     0,     0,     0,   328,   329,   330,   331,
     332,   333,   334,   335,   336,   318,   319,   320,   321,   322,
     323,   324,   325,     0,     0,     0,     0,     0,     0,     0,
     318,   319,   320,   321,     0,   327,   324,   325,     0,   328,
     532,   330,   331,   332,   333,   334,   335,   336,     0,     0,
     327,     0,     0,     0,   328,   329,   330,   331,   332,   333,
     334,   335,   336,   318,   319,   320,   321,     0,     0,   324,
     325,     0,     0,     0,   318,   319,   320,   321,     0,     0,
     324,   325,     0,   327,     0,     0,     0,   328,   329,   330,
     331,   332,   333,   334,   327,   336,     0,     0,   328,   329,
     330,   331,   332,   333,   334,   318,   319,   320,   321,     0,
       0,   324,   325,     0,     0,     0,     0,   320,   321,     0,
       0,   324,   325,     0,     0,   327,     0,     0,     0,   328,
     329,   330,   331,   332,   333,   327,     0,     0,     0,   328,
     329,   330,   331,   332,   333
};

static const yytype_int16 yycheck[] =
{
      25,   112,    31,   113,    69,   213,    94,   219,   215,   216,
     100,    75,    79,     0,    67,    68,     0,    84,   226,   227,
     228,   229,   230,   231,   236,    12,    48,    94,    81,    86,
      94,    47,    19,    26,    87,    88,    89,    90,    10,    26,
      97,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,     3,     4,     5,     6,     7,     8,
       9,    49,    47,    48,    52,   293,    47,   112,   113,   297,
     298,    81,   300,   301,   123,    54,    55,    87,    88,    89,
      90,   130,     3,     4,     5,     6,     7,     8,     9,   100,
     318,   319,   320,   321,   322,   323,   324,   325,    95,   327,
     328,   329,   330,   331,   332,   333,   334,   335,   336,    47,
      97,   339,   340,   341,   342,   343,   344,    94,   131,    88,
      89,    90,    94,    94,   131,   210,     3,     4,     5,     6,
       7,     8,     9,    94,     3,     4,     5,     6,     7,     8,
       9,    10,    94,    12,    13,   101,    95,    94,    94,    94,
      94,   282,    94,    94,   284,    94,    79,    94,    94,    94,
     209,   389,    94,   390,    94,   393,   394,    94,    94,    94,
      47,    48,    94,     3,    95,    94,    94,    94,    47,    94,
      49,    50,    51,    52,    53,    54,    55,    94,    94,     3,
       4,     5,     6,     7,     8,     9,    94,    94,    94,    94,
      69,    94,    71,    72,    73,    74,    94,    76,    77,    95,
      79,    80,    81,    82,    83,   447,    85,   100,   446,   451,
     448,   449,   450,   230,    95,    94,   454,    95,     3,    98,
       3,   100,   101,    47,     3,     3,     3,   282,    47,   284,
      49,    50,    51,    52,    53,     7,     7,     7,    95,   477,
       3,     3,     4,     5,     6,     7,     8,     9,    10,   491,
      12,    13,     3,    95,   492,     3,     6,    76,    77,    95,
       3,    95,    81,    95,     6,     5,    85,     3,     3,     6,
      95,    97,   510,   511,     6,     3,     6,     3,     3,   521,
      99,    99,     6,    97,    94,    47,    47,    49,    50,    51,
      52,    53,    54,    55,   532,     3,     4,     5,     6,     7,
       8,     9,    47,   444,    94,    94,    94,    69,    47,    71,
      72,    73,    74,    47,    76,    77,    47,    79,    80,    81,
      82,    83,    79,    85,     3,     4,     5,     6,     7,     8,
       9,    10,    94,    12,    13,   390,    98,    47,   100,   101,
      47,    47,    47,    47,    47,    47,    47,    47,   101,   490,
      47,    47,    47,    47,    47,    54,    55,    56,    57,    58,
      59,    60,    47,    47,    47,    47,    47,    47,    47,    47,
      49,    50,    51,    52,    53,    54,    55,    47,     3,     4,
       5,     6,     7,     8,     9,    84,    47,    47,    79,   444,
      69,    79,    71,    72,    73,    74,    47,    76,    77,    75,
      79,    80,    81,    82,    83,    79,    85,    73,    79,    95,
      99,    84,    79,     5,     3,    94,     7,     3,    95,    98,
      95,   100,    47,    95,    49,    50,    51,    52,    53,    54,
      55,    95,    95,    95,     8,   490,    47,    95,    49,    50,
      51,    52,    53,    54,    55,    95,    95,    95,    94,    74,
      97,    76,    77,    97,    97,    97,    81,    82,    83,    97,
      85,    97,    95,    74,    95,    76,    77,    95,    79,    94,
      81,    82,    83,    98,    85,    97,    47,    95,    49,    50,
      51,    52,    53,    94,    97,    95,    47,    98,    49,    50,
      51,    52,    53,    54,    55,    97,    47,    97,    49,    50,
      51,    52,    53,    54,    55,    76,    77,    97,    95,     5,
      81,     5,    47,    74,    85,    76,    77,     3,     6,     3,
      81,    82,    83,    74,    85,    76,    77,    98,    47,    47,
      81,    82,    83,    94,    85,    47,    47,    98,    49,    50,
      51,    52,    53,    94,    47,    47,    47,    98,    49,    50,
      51,    52,    53,    54,    55,    47,    47,    47,    49,    50,
      51,    52,    53,    74,    47,    76,    77,    47,    97,    97,
      81,    70,    97,    74,    85,    76,    77,    97,    95,    95,
       3,    82,    83,    94,    97,    76,    77,    98,    97,    97,
      81,    97,    79,    94,    85,     6,     8,    98,    61,    62,
      63,    64,    65,    66,    67,    68,     7,     5,     5,     9,
       6,    86,    47,    95,    47,    47,    47,    47,    81,    47,
      47,    97,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    47,    97,    95,    97,    61,    62,    63,    64,    65,
      66,    67,    68,    95,    95,     5,     5,    97,    97,    97,
       6,     5,     5,    47,    47,    81,    47,    47,    95,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    47,     3,
      95,    97,    61,    62,    63,    64,    65,    66,    67,    68,
      97,    95,    97,     3,    47,    47,    95,    97,     5,    47,
      95,    12,    81,    11,    30,   492,    85,    86,    87,    88,
      89,    90,    91,    92,    93,   284,   393,    -1,    97,    61,
      62,    63,    64,    65,    66,    67,    68,    -1,    -1,   446,
     449,   390,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    81,
      -1,    -1,    -1,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    -1,    -1,    -1,    97,    61,    62,    63,    64,
      65,    66,    67,    68,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    81,    -1,    -1,    -1,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    -1,
      -1,    -1,    97,    61,    62,    63,    64,    65,    66,    67,
      68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    81,    -1,    -1,    -1,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    -1,    -1,    -1,    97,
      61,    62,    63,    64,    65,    66,    67,    68,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      81,    -1,    -1,    -1,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    -1,    95,    61,    62,    63,    64,    65,
      66,    67,    68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    81,    -1,    -1,    -1,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    -1,    95,
      61,    62,    63,    64,    65,    66,    67,    68,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      81,    -1,    -1,    -1,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    -1,    95,    61,    62,    63,    64,    65,
      66,    67,    68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    81,    -1,    -1,    -1,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    -1,    95,
      61,    62,    63,    64,    65,    66,    67,    68,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      81,    -1,    -1,    -1,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    -1,    95,    61,    62,    63,    64,    65,
      66,    67,    68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    81,    -1,    -1,    -1,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    -1,    95,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    61,    62,    63,    64,    65,    66,    67,
      68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    79,    -1,    81,    -1,    -1,    -1,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    61,    62,    63,    64,
      65,    66,    67,    68,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    79,    -1,    81,    -1,    -1,    -1,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    61,
      62,    63,    64,    65,    66,    67,    68,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    79,    -1,    81,
      -1,    -1,    -1,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    61,    62,    63,    64,    65,    66,    67,    68,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    81,    -1,    -1,    -1,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    61,    62,    63,    64,    65,
      66,    67,    68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      61,    62,    63,    64,    -1,    81,    67,    68,    -1,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    -1,    -1,
      81,    -1,    -1,    -1,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    61,    62,    63,    64,    -1,    -1,    67,
      68,    -1,    -1,    -1,    61,    62,    63,    64,    -1,    -1,
      67,    68,    -1,    81,    -1,    -1,    -1,    85,    86,    87,
      88,    89,    90,    91,    81,    93,    -1,    -1,    85,    86,
      87,    88,    89,    90,    91,    61,    62,    63,    64,    -1,
      -1,    67,    68,    -1,    -1,    -1,    -1,    63,    64,    -1,
      -1,    67,    68,    -1,    -1,    81,    -1,    -1,    -1,    85,
      86,    87,    88,    89,    90,    81,    -1,    -1,    -1,    85,
      86,    87,    88,    89,    90
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,     9,    47,    48,
     103,   104,   105,   106,   107,   119,   120,   123,   125,    94,
     100,     0,   123,   104,    79,    84,    94,    47,    10,   124,
     126,    95,   119,   121,   122,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   151,   152,   153,   154,   155,   156,   157,   158,
     159,   160,   161,    47,    49,    50,    51,    52,    53,    76,
      77,    81,    85,    98,   108,   109,   110,   111,   112,   113,
     114,   115,   116,    95,   121,    47,   124,   100,   162,    47,
      95,    97,    94,    94,    94,    94,    94,    94,    94,    94,
      94,    94,    94,    94,    94,    94,    94,    94,    94,    94,
      94,    94,    94,    94,    94,    94,    94,    94,    94,    94,
      94,    94,    94,    94,    94,   101,   127,   162,   162,   162,
     162,   162,   162,   162,   162,   162,   162,   162,   162,   162,
     162,   162,   162,   162,   162,   162,   162,   162,   162,   162,
     162,   162,   162,   162,   162,   162,   162,   162,   162,   162,
      49,    52,   108,    99,   109,   117,   118,    79,   162,    95,
     100,    10,    12,    13,    47,    54,    55,    69,    71,    72,
      73,    74,    76,    77,    79,    80,    81,    82,    83,    85,
      94,    98,   101,   110,   119,   162,   163,   164,   165,   172,
     173,   174,   175,   176,   177,   178,   179,   162,   121,    95,
      95,     3,     3,     3,     3,     3,     3,     7,     7,     7,
      95,     3,     3,    95,     6,     3,    95,     3,    95,     6,
       5,     3,     3,     6,     6,    95,    95,     3,     6,     3,
       3,     6,    97,    99,    97,   162,   127,    47,    48,    47,
      79,   172,    75,    94,    47,   179,   179,    94,    94,   164,
      94,    94,    47,    49,    52,   172,   172,   172,   172,   119,
     172,   170,   171,   172,    47,   101,   164,    79,    61,    62,
      63,    64,    65,    66,    67,    68,    79,    81,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    54,    55,    56,
      57,    58,    59,    60,    84,    47,    47,    47,    47,    47,
      47,    47,    47,    47,    47,    47,    47,    47,    47,    47,
      47,    47,    47,    47,    47,    47,    47,    47,    47,    47,
     108,   117,   101,    79,    79,    79,    79,    47,   168,   169,
     172,   172,   166,   167,   172,    73,   172,   172,    79,    97,
      95,    95,    99,    97,    84,   172,   172,   172,   172,   172,
     172,   172,   172,   172,   172,   172,   172,   172,   172,   172,
     172,   172,   172,   172,   172,   172,   172,   172,   172,    95,
      95,    95,    95,    95,    95,    95,    95,    95,    97,    95,
      97,    97,    97,    97,    97,    95,    95,    95,    97,    95,
      95,    97,    97,    97,    97,    95,    97,    95,    79,    97,
      94,    95,    95,   172,    94,   110,   175,   179,   171,   172,
       5,     3,     7,     8,     3,     5,     5,     3,     6,     3,
     108,   169,   164,   172,   167,   172,   164,    97,   172,    47,
      47,    47,    47,    47,    47,    47,    47,    47,    47,    86,
      97,    70,    79,    95,   172,    95,    97,    97,    97,    97,
      95,    97,    95,    97,    97,    97,   108,   164,   166,    79,
      86,    97,     6,     3,     7,     8,     5,     5,     6,     9,
      86,    95,   172,    47,    47,    47,    47,    47,    47,    47,
      47,   164,    86,    97,    95,    95,    95,    97,    97,    97,
      97,     5,     5,     6,     5,     5,    47,    47,    47,    47,
      47,    95,    97,    95,    97,    95,     3,     3,    47,    47,
      95,    97,     5,    47,    95
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 271 "indra.y"
    {
		(yyval.script) = new LLScriptScript((yyvsp[(1) - (2)].global_store), (yyvsp[(2) - (2)].state));
		gAllocationManager->addAllocation((yyval.script));
		gScriptp = (yyval.script);
	}
    break;

  case 3:
#line 277 "indra.y"
    {
		(yyval.script) = new LLScriptScript(NULL, (yyvsp[(1) - (1)].state));
		gAllocationManager->addAllocation((yyval.script));
		gScriptp = (yyval.script);
	}
    break;

  case 4:
#line 286 "indra.y"
    {
		(yyval.global_store) = (yyvsp[(1) - (1)].global_store);
	}
    break;

  case 5:
#line 290 "indra.y"
    {
		(yyval.global_store) = (yyvsp[(1) - (2)].global_store);
		(yyvsp[(1) - (2)].global_store)->addGlobal((yyvsp[(2) - (2)].global_store));
	}
    break;

  case 6:
#line 298 "indra.y"
    {
		(yyval.global_store) = new LLScritpGlobalStorage((yyvsp[(1) - (1)].global));
		gAllocationManager->addAllocation((yyval.global_store));
	}
    break;

  case 7:
#line 303 "indra.y"
    {
		(yyval.global_store) = new LLScritpGlobalStorage((yyvsp[(1) - (1)].global_funcs));
		gAllocationManager->addAllocation((yyval.global_store));
	}
    break;

  case 8:
#line 311 "indra.y"
    {
		(yyval.identifier) = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(2) - (2)].sval), (yyvsp[(1) - (2)].type));	
		gAllocationManager->addAllocation((yyval.identifier));
	}
    break;

  case 9:
#line 319 "indra.y"
    {
		(yyval.global) = new LLScriptGlobalVariable(gLine, gColumn, (yyvsp[(1) - (2)].identifier)->mType, (yyvsp[(1) - (2)].identifier), NULL);
		gAllocationManager->addAllocation((yyval.global));
	}
    break;

  case 10:
#line 324 "indra.y"
    {
		(yyval.global) = new LLScriptGlobalVariable(gLine, gColumn, (yyvsp[(1) - (4)].identifier)->mType, (yyvsp[(1) - (4)].identifier), (yyvsp[(3) - (4)].assignable));
		gAllocationManager->addAllocation((yyval.global));
	}
    break;

  case 11:
#line 332 "indra.y"
    {
		(yyval.assignable) = (yyvsp[(1) - (1)].assignable);
	}
    break;

  case 12:
#line 336 "indra.y"
    {
		(yyval.assignable) = (yyvsp[(1) - (1)].assignable);
	}
    break;

  case 13:
#line 343 "indra.y"
    {
		LLScriptIdentifier	*id = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(1) - (1)].sval));	
		gAllocationManager->addAllocation(id);
		(yyval.assignable) = new LLScriptSAIdentifier(gLine, gColumn, id);	
		gAllocationManager->addAllocation((yyval.assignable));
	}
    break;

  case 14:
#line 350 "indra.y"
    {
		(yyval.assignable) = new LLScriptSAConstant(gLine, gColumn, (yyvsp[(1) - (1)].constant));
		gAllocationManager->addAllocation((yyval.assignable));
	}
    break;

  case 15:
#line 355 "indra.y"
    {
		(yyval.assignable) = (yyvsp[(1) - (1)].assignable);
	}
    break;

  case 16:
#line 362 "indra.y"
    {
		(yyval.constant) = new LLScriptConstantInteger(gLine, gColumn, (yyvsp[(1) - (1)].ival));
		gAllocationManager->addAllocation((yyval.constant));
	}
    break;

  case 17:
#line 367 "indra.y"
    {
		(yyval.constant) = new LLScriptConstantFloat(gLine, gColumn, (yyvsp[(1) - (1)].fval));
		gAllocationManager->addAllocation((yyval.constant));
	}
    break;

  case 18:
#line 372 "indra.y"
    {
		(yyval.constant) = new LLScriptConstantString(gLine, gColumn, (yyvsp[(1) - (1)].sval));
		gAllocationManager->addAllocation((yyval.constant));
	}
    break;

  case 19:
#line 380 "indra.y"
    {
		(yyval.fval) = (yyvsp[(1) - (1)].fval);
	}
    break;

  case 20:
#line 384 "indra.y"
    {
		(yyval.fval) = -(yyvsp[(2) - (2)].fval);
	}
    break;

  case 21:
#line 391 "indra.y"
    {
		(yyval.ival) = (yyvsp[(1) - (1)].ival);
	}
    break;

  case 22:
#line 395 "indra.y"
    {
		(yyval.ival) = (yyvsp[(1) - (1)].ival);
	}
    break;

  case 23:
#line 399 "indra.y"
    {
		(yyval.ival) = (yyvsp[(1) - (1)].ival);
	}
    break;

  case 24:
#line 403 "indra.y"
    {
		(yyval.ival) = -(yyvsp[(2) - (2)].ival);
	}
    break;

  case 25:
#line 410 "indra.y"
    {
		(yyval.assignable) = (yyvsp[(1) - (1)].assignable);
	}
    break;

  case 26:
#line 414 "indra.y"
    {
		(yyval.assignable) = (yyvsp[(1) - (1)].assignable);
	}
    break;

  case 27:
#line 421 "indra.y"
    {
		(yyval.assignable) = new LLScriptSAVector(gLine, gColumn, (yyvsp[(2) - (7)].assignable), (yyvsp[(4) - (7)].assignable), (yyvsp[(6) - (7)].assignable));
		gAllocationManager->addAllocation((yyval.assignable));
	}
    break;

  case 28:
#line 426 "indra.y"
    {
		LLScriptConstantFloat *cf0 = new LLScriptConstantFloat(gLine, gColumn, 0.f);
		gAllocationManager->addAllocation(cf0);
		LLScriptSAConstant *sa0 = new LLScriptSAConstant(gLine, gColumn, cf0);
		gAllocationManager->addAllocation(sa0);
		LLScriptConstantFloat *cf1 = new LLScriptConstantFloat(gLine, gColumn, 0.f);
		gAllocationManager->addAllocation(cf1);
		LLScriptSAConstant *sa1 = new LLScriptSAConstant(gLine, gColumn, cf1);
		gAllocationManager->addAllocation(sa1);
		LLScriptConstantFloat *cf2 = new LLScriptConstantFloat(gLine, gColumn, 0.f);
		gAllocationManager->addAllocation(cf2);
		LLScriptSAConstant *sa2 = new LLScriptSAConstant(gLine, gColumn, cf2);
		gAllocationManager->addAllocation(sa2);
		(yyval.assignable) = new LLScriptSAVector(gLine, gColumn, sa0, sa1, sa2);
		gAllocationManager->addAllocation((yyval.assignable));
	}
    break;

  case 29:
#line 446 "indra.y"
    {
		(yyval.assignable) = new LLScriptSAQuaternion(gLine, gColumn, (yyvsp[(2) - (9)].assignable), (yyvsp[(4) - (9)].assignable), (yyvsp[(6) - (9)].assignable), (yyvsp[(8) - (9)].assignable));
		gAllocationManager->addAllocation((yyval.assignable));
	}
    break;

  case 30:
#line 451 "indra.y"
    {
		LLScriptConstantFloat *cf0 = new LLScriptConstantFloat(gLine, gColumn, 0.f);
		gAllocationManager->addAllocation(cf0);
		LLScriptSAConstant *sa0 = new LLScriptSAConstant(gLine, gColumn, cf0);
		gAllocationManager->addAllocation(sa0);
		LLScriptConstantFloat *cf1 = new LLScriptConstantFloat(gLine, gColumn, 0.f);
		gAllocationManager->addAllocation(cf1);
		LLScriptSAConstant *sa1 = new LLScriptSAConstant(gLine, gColumn, cf1);
		gAllocationManager->addAllocation(sa1);
		LLScriptConstantFloat *cf2 = new LLScriptConstantFloat(gLine, gColumn, 0.f);
		gAllocationManager->addAllocation(cf2);
		LLScriptSAConstant *sa2 = new LLScriptSAConstant(gLine, gColumn, cf2);
		gAllocationManager->addAllocation(sa2);
		LLScriptConstantFloat *cf3 = new LLScriptConstantFloat(gLine, gColumn, 1.f);
		gAllocationManager->addAllocation(cf3);
		LLScriptSAConstant *sa3 = new LLScriptSAConstant(gLine, gColumn, cf3);
		gAllocationManager->addAllocation(sa3);
		(yyval.assignable) = new LLScriptSAQuaternion(gLine, gColumn, sa0, sa1, sa2, sa3);
		gAllocationManager->addAllocation((yyval.assignable));
	}
    break;

  case 31:
#line 475 "indra.y"
    {
		(yyval.assignable) = new LLScriptSAList(gLine, gColumn, (yyvsp[(2) - (3)].assignable));
		gAllocationManager->addAllocation((yyval.assignable));
	}
    break;

  case 32:
#line 480 "indra.y"
    {
		(yyval.assignable) = new LLScriptSAList(gLine, gColumn, NULL);
		gAllocationManager->addAllocation((yyval.assignable));
	}
    break;

  case 33:
#line 488 "indra.y"
    {
		(yyval.assignable) = (yyvsp[(1) - (1)].assignable);
	}
    break;

  case 34:
#line 492 "indra.y"
    {
		(yyval.assignable) = (yyvsp[(1) - (3)].assignable);
		(yyvsp[(1) - (3)].assignable)->addAssignable((yyvsp[(3) - (3)].assignable));
	}
    break;

  case 35:
#line 500 "indra.y"
    {
		(yyval.assignable) = (yyvsp[(1) - (1)].assignable);
	}
    break;

  case 36:
#line 507 "indra.y"
    {  
		(yyval.type) = new LLScriptType(gLine, gColumn, LST_INTEGER);
		gAllocationManager->addAllocation((yyval.type));
	}
    break;

  case 37:
#line 512 "indra.y"
    {  
		(yyval.type) = new LLScriptType(gLine, gColumn, LST_FLOATINGPOINT);
		gAllocationManager->addAllocation((yyval.type));
	}
    break;

  case 38:
#line 517 "indra.y"
    {  
		(yyval.type) = new LLScriptType(gLine, gColumn, LST_STRING);
		gAllocationManager->addAllocation((yyval.type));
	}
    break;

  case 39:
#line 522 "indra.y"
    {  
		(yyval.type) = new LLScriptType(gLine, gColumn, LST_KEY);
		gAllocationManager->addAllocation((yyval.type));
	}
    break;

  case 40:
#line 527 "indra.y"
    {  
		(yyval.type) = new LLScriptType(gLine, gColumn, LST_VECTOR);
		gAllocationManager->addAllocation((yyval.type));
	}
    break;

  case 41:
#line 532 "indra.y"
    {  
		(yyval.type) = new LLScriptType(gLine, gColumn, LST_QUATERNION);
		gAllocationManager->addAllocation((yyval.type));
	}
    break;

  case 42:
#line 537 "indra.y"
    {
		(yyval.type) = new LLScriptType(gLine, gColumn, LST_LIST);
		gAllocationManager->addAllocation((yyval.type));
	}
    break;

  case 43:
#line 545 "indra.y"
    {  
		LLScriptIdentifier	*id = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(1) - (4)].sval));	
		gAllocationManager->addAllocation(id);
		(yyval.global_funcs) = new LLScriptGlobalFunctions(gLine, gColumn, NULL, id, NULL, (yyvsp[(4) - (4)].statement));
		gAllocationManager->addAllocation((yyval.global_funcs));
	}
    break;

  case 44:
#line 552 "indra.y"
    {
		(yyval.global_funcs) = new LLScriptGlobalFunctions(gLine, gColumn, (yyvsp[(1) - (4)].identifier)->mType, (yyvsp[(1) - (4)].identifier), NULL, (yyvsp[(4) - (4)].statement));
		gAllocationManager->addAllocation((yyval.global_funcs));
	}
    break;

  case 45:
#line 557 "indra.y"
    {
		LLScriptIdentifier	*id = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(1) - (5)].sval));	
		gAllocationManager->addAllocation(id);
		(yyval.global_funcs) = new LLScriptGlobalFunctions(gLine, gColumn, NULL, id, (yyvsp[(3) - (5)].global_decl), (yyvsp[(5) - (5)].statement));
		gAllocationManager->addAllocation((yyval.global_funcs));
	}
    break;

  case 46:
#line 564 "indra.y"
    {  
		(yyval.global_funcs) = new LLScriptGlobalFunctions(gLine, gColumn, (yyvsp[(1) - (5)].identifier)->mType, (yyvsp[(1) - (5)].identifier), (yyvsp[(3) - (5)].global_decl), (yyvsp[(5) - (5)].statement));
		gAllocationManager->addAllocation((yyval.global_funcs));
	}
    break;

  case 47:
#line 572 "indra.y"
    {  
		(yyval.global_decl) = (yyvsp[(1) - (1)].global_decl);
	}
    break;

  case 48:
#line 576 "indra.y"
    {  
		(yyval.global_decl) = (yyvsp[(1) - (3)].global_decl);
		(yyvsp[(1) - (3)].global_decl)->addFunctionParameter((yyvsp[(3) - (3)].global_decl));
	}
    break;

  case 49:
#line 584 "indra.y"
    {  
		LLScriptIdentifier	*id = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(2) - (2)].sval));	
		gAllocationManager->addAllocation(id);
		(yyval.global_decl) = new LLScriptFunctionDec(gLine, gColumn, (yyvsp[(1) - (2)].type), id);
		gAllocationManager->addAllocation((yyval.global_decl));
	}
    break;

  case 50:
#line 594 "indra.y"
    {  
		(yyval.state) = (yyvsp[(1) - (1)].state);
	}
    break;

  case 51:
#line 598 "indra.y"
    {  
		(yyval.state) = (yyvsp[(1) - (2)].state);
		(yyvsp[(1) - (2)].state)->mNextp = (yyvsp[(2) - (2)].state);
	}
    break;

  case 52:
#line 606 "indra.y"
    {  
		(yyval.state) = (yyvsp[(1) - (1)].state);
	}
    break;

  case 53:
#line 610 "indra.y"
    {  
		(yyval.state) = (yyvsp[(1) - (2)].state);
		(yyvsp[(1) - (2)].state)->addState((yyvsp[(2) - (2)].state));
	}
    break;

  case 54:
#line 618 "indra.y"
    {  
		LLScriptIdentifier	*id = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(1) - (4)].sval));	
		gAllocationManager->addAllocation(id);
		(yyval.state) = new LLScriptState(gLine, gColumn, LSSTYPE_DEFAULT, id, (yyvsp[(3) - (4)].handler));
		gAllocationManager->addAllocation((yyval.state));
	}
    break;

  case 55:
#line 628 "indra.y"
    {  
		LLScriptIdentifier	*id = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(2) - (5)].sval));	
		gAllocationManager->addAllocation(id);
		(yyval.state) = new LLScriptState(gLine, gColumn, LSSTYPE_USER, id, (yyvsp[(4) - (5)].handler));
		gAllocationManager->addAllocation((yyval.state));
	}
    break;

  case 56:
#line 638 "indra.y"
    {  
		(yyval.handler) = (yyvsp[(1) - (1)].handler);
	}
    break;

  case 57:
#line 642 "indra.y"
    {  
		(yyval.handler) = (yyvsp[(1) - (2)].handler);
		(yyvsp[(1) - (2)].handler)->addEvent((yyvsp[(2) - (2)].handler));
	}
    break;

  case 58:
#line 650 "indra.y"
    {  
		(yyval.handler) = new LLScriptEventHandler(gLine, gColumn, (yyvsp[(1) - (2)].event), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.handler));
	}
    break;

  case 59:
#line 655 "indra.y"
    {  
		(yyval.handler) = new LLScriptEventHandler(gLine, gColumn, (yyvsp[(1) - (2)].event), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.handler));
	}
    break;

  case 60:
#line 660 "indra.y"
    {
		(yyval.handler) = new LLScriptEventHandler(gLine, gColumn, (yyvsp[(1) - (2)].event), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.handler));
	}
    break;

  case 61:
#line 665 "indra.y"
    {  
		(yyval.handler) = new LLScriptEventHandler(gLine, gColumn, (yyvsp[(1) - (2)].event), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.handler));
	}
    break;

  case 62:
#line 670 "indra.y"
    {  
		(yyval.handler) = new LLScriptEventHandler(gLine, gColumn, (yyvsp[(1) - (2)].event), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.handler));
	}
    break;

  case 63:
#line 675 "indra.y"
    {  
		(yyval.handler) = new LLScriptEventHandler(gLine, gColumn, (yyvsp[(1) - (2)].event), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.handler));
	}
    break;

  case 64:
#line 680 "indra.y"
    {  
		(yyval.handler) = new LLScriptEventHandler(gLine, gColumn, (yyvsp[(1) - (2)].event), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.handler));
	}
    break;

  case 65:
#line 685 "indra.y"
    {  
		(yyval.handler) = new LLScriptEventHandler(gLine, gColumn, (yyvsp[(1) - (2)].event), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.handler));
	}
    break;

  case 66:
#line 690 "indra.y"
    {  
		(yyval.handler) = new LLScriptEventHandler(gLine, gColumn, (yyvsp[(1) - (2)].event), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.handler));
	}
    break;

  case 67:
#line 695 "indra.y"
    {  
		(yyval.handler) = new LLScriptEventHandler(gLine, gColumn, (yyvsp[(1) - (2)].event), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.handler));
	}
    break;

  case 68:
#line 700 "indra.y"
    {  
		(yyval.handler) = new LLScriptEventHandler(gLine, gColumn, (yyvsp[(1) - (2)].event), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.handler));
	}
    break;

  case 69:
#line 705 "indra.y"
    {  
		(yyval.handler) = new LLScriptEventHandler(gLine, gColumn, (yyvsp[(1) - (2)].event), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.handler));
	}
    break;

  case 70:
#line 710 "indra.y"
    {
		(yyval.handler) = new LLScriptEventHandler(gLine, gColumn, (yyvsp[(1) - (2)].event), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.handler));
	}
    break;

  case 71:
#line 715 "indra.y"
    {  
		(yyval.handler) = new LLScriptEventHandler(gLine, gColumn, (yyvsp[(1) - (2)].event), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.handler));
	}
    break;

  case 72:
#line 720 "indra.y"
    {  
		(yyval.handler) = new LLScriptEventHandler(gLine, gColumn, (yyvsp[(1) - (2)].event), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.handler));
	}
    break;

  case 73:
#line 725 "indra.y"
    {  
		(yyval.handler) = new LLScriptEventHandler(gLine, gColumn, (yyvsp[(1) - (2)].event), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.handler));
	}
    break;

  case 74:
#line 730 "indra.y"
    {  
		(yyval.handler) = new LLScriptEventHandler(gLine, gColumn, (yyvsp[(1) - (2)].event), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.handler));
	}
    break;

  case 75:
#line 735 "indra.y"
    {  
		(yyval.handler) = new LLScriptEventHandler(gLine, gColumn, (yyvsp[(1) - (2)].event), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.handler));
	}
    break;

  case 76:
#line 740 "indra.y"
    {  
		(yyval.handler) = new LLScriptEventHandler(gLine, gColumn, (yyvsp[(1) - (2)].event), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.handler));
	}
    break;

  case 77:
#line 745 "indra.y"
    {  
		(yyval.handler) = new LLScriptEventHandler(gLine, gColumn, (yyvsp[(1) - (2)].event), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.handler));
	}
    break;

  case 78:
#line 750 "indra.y"
    {  
		(yyval.handler) = new LLScriptEventHandler(gLine, gColumn, (yyvsp[(1) - (2)].event), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.handler));
	}
    break;

  case 79:
#line 755 "indra.y"
    {  
		(yyval.handler) = new LLScriptEventHandler(gLine, gColumn, (yyvsp[(1) - (2)].event), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.handler));
	}
    break;

  case 80:
#line 760 "indra.y"
    {  
		(yyval.handler) = new LLScriptEventHandler(gLine, gColumn, (yyvsp[(1) - (2)].event), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.handler));
	}
    break;

  case 81:
#line 765 "indra.y"
    {  
		(yyval.handler) = new LLScriptEventHandler(gLine, gColumn, (yyvsp[(1) - (2)].event), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.handler));
	}
    break;

  case 82:
#line 770 "indra.y"
    {  
		(yyval.handler) = new LLScriptEventHandler(gLine, gColumn, (yyvsp[(1) - (2)].event), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.handler));
	}
    break;

  case 83:
#line 775 "indra.y"
    {  
		(yyval.handler) = new LLScriptEventHandler(gLine, gColumn, (yyvsp[(1) - (2)].event), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.handler));
	}
    break;

  case 84:
#line 780 "indra.y"
    {  
		(yyval.handler) = new LLScriptEventHandler(gLine, gColumn, (yyvsp[(1) - (2)].event), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.handler));
	}
    break;

  case 85:
#line 785 "indra.y"
    {  
		(yyval.handler) = new LLScriptEventHandler(gLine, gColumn, (yyvsp[(1) - (2)].event), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.handler));
	}
    break;

  case 86:
#line 790 "indra.y"
    {  
		(yyval.handler) = new LLScriptEventHandler(gLine, gColumn, (yyvsp[(1) - (2)].event), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.handler));
	}
    break;

  case 87:
#line 795 "indra.y"
    {  
		(yyval.handler) = new LLScriptEventHandler(gLine, gColumn, (yyvsp[(1) - (2)].event), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.handler));
	}
    break;

  case 88:
#line 800 "indra.y"
    {  
		(yyval.handler) = new LLScriptEventHandler(gLine, gColumn, (yyvsp[(1) - (2)].event), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.handler));
	}
    break;

  case 89:
#line 805 "indra.y"
    {  
		(yyval.handler) = new LLScriptEventHandler(gLine, gColumn, (yyvsp[(1) - (2)].event), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.handler));
	}
    break;

  case 90:
#line 810 "indra.y"
    {  
		(yyval.handler) = new LLScriptEventHandler(gLine, gColumn, (yyvsp[(1) - (2)].event), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.handler));
	}
    break;

  case 91:
#line 818 "indra.y"
    {  
		(yyval.event) = new LLScriptStateEntryEvent(gLine, gColumn);
		gAllocationManager->addAllocation((yyval.event));
	}
    break;

  case 92:
#line 826 "indra.y"
    {  
		(yyval.event) = new LLScriptStateExitEvent(gLine, gColumn);
		gAllocationManager->addAllocation((yyval.event));
	}
    break;

  case 93:
#line 834 "indra.y"
    {  
		LLScriptIdentifier	*id1 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(4) - (5)].sval));	
		gAllocationManager->addAllocation(id1);
		(yyval.event) = new LLScriptTouchStartEvent(gLine, gColumn, id1);
		gAllocationManager->addAllocation((yyval.event));
	}
    break;

  case 94:
#line 844 "indra.y"
    {  
		LLScriptIdentifier	*id1 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(4) - (5)].sval));	
		gAllocationManager->addAllocation(id1);
		(yyval.event) = new LLScriptTouchEvent(gLine, gColumn, id1);
		gAllocationManager->addAllocation((yyval.event));
	}
    break;

  case 95:
#line 854 "indra.y"
    {  
		LLScriptIdentifier	*id1 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(4) - (5)].sval));	
		gAllocationManager->addAllocation(id1);
		(yyval.event) = new LLScriptTouchEndEvent(gLine, gColumn, id1);
		gAllocationManager->addAllocation((yyval.event));
	}
    break;

  case 96:
#line 864 "indra.y"
    {  
		LLScriptIdentifier	*id1 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(4) - (5)].sval));	
		gAllocationManager->addAllocation(id1);
		(yyval.event) = new LLScriptCollisionStartEvent(gLine, gColumn, id1);
		gAllocationManager->addAllocation((yyval.event));
	}
    break;

  case 97:
#line 874 "indra.y"
    {  
		LLScriptIdentifier	*id1 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(4) - (5)].sval));	
		gAllocationManager->addAllocation(id1);
		(yyval.event) = new LLScriptCollisionEvent(gLine, gColumn, id1);
		gAllocationManager->addAllocation((yyval.event));
	}
    break;

  case 98:
#line 884 "indra.y"
    {  
		LLScriptIdentifier	*id1 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(4) - (5)].sval));	
		gAllocationManager->addAllocation(id1);
		(yyval.event) = new LLScriptCollisionEndEvent(gLine, gColumn, id1);
		gAllocationManager->addAllocation((yyval.event));
	}
    break;

  case 99:
#line 894 "indra.y"
    {  
		LLScriptIdentifier	*id1 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(4) - (5)].sval));	
		gAllocationManager->addAllocation(id1);
		(yyval.event) = new LLScriptLandCollisionStartEvent(gLine, gColumn, id1);
		gAllocationManager->addAllocation((yyval.event));
	}
    break;

  case 100:
#line 904 "indra.y"
    {  
		LLScriptIdentifier	*id1 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(4) - (5)].sval));	
		gAllocationManager->addAllocation(id1);
		(yyval.event) = new LLScriptLandCollisionEvent(gLine, gColumn, id1);
		gAllocationManager->addAllocation((yyval.event));
	}
    break;

  case 101:
#line 914 "indra.y"
    {  
		LLScriptIdentifier	*id1 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(4) - (5)].sval));	
		gAllocationManager->addAllocation(id1);
		(yyval.event) = new LLScriptLandCollisionEndEvent(gLine, gColumn, id1);
		gAllocationManager->addAllocation((yyval.event));
	}
    break;

  case 102:
#line 924 "indra.y"
    {  
		LLScriptIdentifier	*id1 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(4) - (11)].sval));	
		gAllocationManager->addAllocation(id1);
		LLScriptIdentifier	*id2 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(7) - (11)].sval));	
		gAllocationManager->addAllocation(id2);
		LLScriptIdentifier	*id3 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(10) - (11)].sval));	
		gAllocationManager->addAllocation(id3);
		(yyval.event) = new LLScriptAtTarget(gLine, gColumn, id1, id2, id3);
		gAllocationManager->addAllocation((yyval.event));
	}
    break;

  case 103:
#line 938 "indra.y"
    {  
		(yyval.event) = new LLScriptNotAtTarget(gLine, gColumn);
		gAllocationManager->addAllocation((yyval.event));
	}
    break;

  case 104:
#line 946 "indra.y"
    {  
		LLScriptIdentifier	*id1 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(4) - (11)].sval));	
		gAllocationManager->addAllocation(id1);
		LLScriptIdentifier	*id2 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(7) - (11)].sval));	
		gAllocationManager->addAllocation(id2);
		LLScriptIdentifier	*id3 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(10) - (11)].sval));	
		gAllocationManager->addAllocation(id3);
		(yyval.event) = new LLScriptAtRotTarget(gLine, gColumn, id1, id2, id3);
		gAllocationManager->addAllocation((yyval.event));
	}
    break;

  case 105:
#line 960 "indra.y"
    {  
		(yyval.event) = new LLScriptNotAtRotTarget(gLine, gColumn);
		gAllocationManager->addAllocation((yyval.event));
	}
    break;

  case 106:
#line 968 "indra.y"
    {  
		LLScriptIdentifier	*id1 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(4) - (8)].sval));	
		gAllocationManager->addAllocation(id1);
		LLScriptIdentifier	*id2 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(7) - (8)].sval));	
		gAllocationManager->addAllocation(id2);
		(yyval.event) = new LLScriptMoneyEvent(gLine, gColumn, id1, id2);
		gAllocationManager->addAllocation((yyval.event));
	}
    break;

  case 107:
#line 980 "indra.y"
    {  
		LLScriptIdentifier	*id1 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(4) - (17)].sval));	
		gAllocationManager->addAllocation(id1);
		LLScriptIdentifier	*id2 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(7) - (17)].sval));	
		gAllocationManager->addAllocation(id2);
		LLScriptIdentifier	*id3 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(10) - (17)].sval));	
		gAllocationManager->addAllocation(id3);
		LLScriptIdentifier	*id4 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(13) - (17)].sval));	
		gAllocationManager->addAllocation(id4);
		LLScriptIdentifier	*id5 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(16) - (17)].sval));	
		gAllocationManager->addAllocation(id5);
		(yyval.event) = new LLScriptEmailEvent(gLine, gColumn, id1, id2, id3, id4, id5);
		gAllocationManager->addAllocation((yyval.event));
	}
    break;

  case 108:
#line 998 "indra.y"
    {  
		LLScriptIdentifier	*id1 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(4) - (5)].sval));	
		gAllocationManager->addAllocation(id1);
		(yyval.event) = new LLScriptRTPEvent(gLine, gColumn, id1);
		gAllocationManager->addAllocation((yyval.event));
	}
    break;

  case 109:
#line 1008 "indra.y"
    {  
		LLScriptIdentifier	*id1 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(4) - (5)].sval));	
		gAllocationManager->addAllocation(id1);
		(yyval.event) = new LLScriptInventoryEvent(gLine, gColumn, id1);
		gAllocationManager->addAllocation((yyval.event));
	}
    break;

  case 110:
#line 1018 "indra.y"
    {  
		LLScriptIdentifier	*id1 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(4) - (5)].sval));	
		gAllocationManager->addAllocation(id1);
		(yyval.event) = new LLScriptAttachEvent(gLine, gColumn, id1);
		gAllocationManager->addAllocation((yyval.event));
	}
    break;

  case 111:
#line 1028 "indra.y"
    {  
		LLScriptIdentifier	*id1 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(4) - (8)].sval));	
		gAllocationManager->addAllocation(id1);
		LLScriptIdentifier	*id2 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(7) - (8)].sval));	
		gAllocationManager->addAllocation(id2);
		(yyval.event) = new LLScriptDataserverEvent(gLine, gColumn, id1, id2);
		gAllocationManager->addAllocation((yyval.event));
	}
    break;

  case 112:
#line 1040 "indra.y"
    {  
		(yyval.event) = new LLScriptMovingStartEvent(gLine, gColumn);
		gAllocationManager->addAllocation((yyval.event));
	}
    break;

  case 113:
#line 1048 "indra.y"
    {  
		(yyval.event) = new LLScriptMovingEndEvent(gLine, gColumn);
		gAllocationManager->addAllocation((yyval.event));
	}
    break;

  case 114:
#line 1056 "indra.y"
    {  
		(yyval.event) = new LLScriptTimerEvent(gLine, gColumn);
		gAllocationManager->addAllocation((yyval.event));
	}
    break;

  case 115:
#line 1064 "indra.y"
    {  
		LLScriptIdentifier	*id1 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(4) - (14)].sval));	
		gAllocationManager->addAllocation(id1);
		LLScriptIdentifier	*id2 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(7) - (14)].sval));	
		gAllocationManager->addAllocation(id2);
		LLScriptIdentifier	*id3 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(10) - (14)].sval));	
		gAllocationManager->addAllocation(id3);
		LLScriptIdentifier	*id4 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(13) - (14)].sval));	
		gAllocationManager->addAllocation(id4);
		(yyval.event) = new LLScriptChatEvent(gLine, gColumn, id1, id2, id3, id4);
		gAllocationManager->addAllocation((yyval.event));
	}
    break;

  case 116:
#line 1080 "indra.y"
    {  
		LLScriptIdentifier	*id1 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(4) - (5)].sval));	
		gAllocationManager->addAllocation(id1);
		(yyval.event) = new LLScriptSensorEvent(gLine, gColumn, id1);
		gAllocationManager->addAllocation((yyval.event));
	}
    break;

  case 117:
#line 1090 "indra.y"
    {  
		(yyval.event) = new LLScriptNoSensorEvent(gLine, gColumn);
		gAllocationManager->addAllocation((yyval.event));
	}
    break;

  case 118:
#line 1098 "indra.y"
    {  
		LLScriptIdentifier	*id1 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(4) - (11)].sval));	
		gAllocationManager->addAllocation(id1);
		LLScriptIdentifier	*id2 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(7) - (11)].sval));	
		gAllocationManager->addAllocation(id2);
		LLScriptIdentifier	*id3 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(10) - (11)].sval));	
		gAllocationManager->addAllocation(id3);
		(yyval.event) = new LLScriptControlEvent(gLine, gColumn, id1, id2, id3);
		gAllocationManager->addAllocation((yyval.event));
	}
    break;

  case 119:
#line 1112 "indra.y"
    {  
		LLScriptIdentifier	*id1 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(4) - (5)].sval));	
		gAllocationManager->addAllocation(id1);
		(yyval.event) = new LLScriptRezEvent(gLine, gColumn, id1);
		gAllocationManager->addAllocation((yyval.event));
	}
    break;

  case 120:
#line 1122 "indra.y"
    {  
		LLScriptIdentifier	*id1 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(4) - (5)].sval));	
		gAllocationManager->addAllocation(id1);
		(yyval.event) = new LLScriptObjectRezEvent(gLine, gColumn, id1);
		gAllocationManager->addAllocation((yyval.event));
	}
    break;

  case 121:
#line 1132 "indra.y"
    {  
		LLScriptIdentifier	*id1 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(4) - (14)].sval));	
		gAllocationManager->addAllocation(id1);
		LLScriptIdentifier	*id2 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(7) - (14)].sval));	
		gAllocationManager->addAllocation(id2);
		LLScriptIdentifier	*id3 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(10) - (14)].sval));	
		gAllocationManager->addAllocation(id3);
		LLScriptIdentifier	*id4 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(13) - (14)].sval));	
		gAllocationManager->addAllocation(id4);
		(yyval.event) = new LLScriptLinkMessageEvent(gLine, gColumn, id1, id2, id3, id4);
		gAllocationManager->addAllocation((yyval.event));
	}
    break;

  case 122:
#line 1148 "indra.y"
    {  
		LLScriptIdentifier	*id1 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(4) - (20)].sval));	
		gAllocationManager->addAllocation(id1);
		LLScriptIdentifier	*id2 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(7) - (20)].sval));	
		gAllocationManager->addAllocation(id2);
		LLScriptIdentifier	*id3 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(10) - (20)].sval));	
		gAllocationManager->addAllocation(id3);
		LLScriptIdentifier	*id4 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(13) - (20)].sval));	
		gAllocationManager->addAllocation(id4);
		LLScriptIdentifier	*id5 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(16) - (20)].sval));	
		gAllocationManager->addAllocation(id4);
		LLScriptIdentifier	*id6 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(19) - (20)].sval));	
		gAllocationManager->addAllocation(id4);
		(yyval.event) = new LLScriptRemoteEvent(gLine, gColumn, id1, id2, id3, id4, id5, id6);
		gAllocationManager->addAllocation((yyval.event));
	}
    break;

  case 123:
#line 1168 "indra.y"
    {  
		LLScriptIdentifier	*id1 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(4) - (14)].sval));	
		gAllocationManager->addAllocation(id1);
		LLScriptIdentifier	*id2 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(7) - (14)].sval));	
		gAllocationManager->addAllocation(id2);
		LLScriptIdentifier	*id3 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(10) - (14)].sval));	
		gAllocationManager->addAllocation(id3);
		LLScriptIdentifier	*id4 = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(13) - (14)].sval));	
		gAllocationManager->addAllocation(id4);
		(yyval.event) = new LLScriptHTTPResponseEvent(gLine, gColumn, id1, id2, id3, id4);
		gAllocationManager->addAllocation((yyval.event));
	}
    break;

  case 124:
#line 1184 "indra.y"
    {  
		(yyval.statement) = new LLScriptCompoundStatement(gLine, gColumn, NULL);
		gAllocationManager->addAllocation((yyval.statement));
	}
    break;

  case 125:
#line 1189 "indra.y"
    {  
		(yyval.statement) = new LLScriptCompoundStatement(gLine, gColumn, (yyvsp[(2) - (3)].statement));
		gAllocationManager->addAllocation((yyval.statement));
	}
    break;

  case 126:
#line 1197 "indra.y"
    {  
		(yyval.statement) = (yyvsp[(1) - (1)].statement);
	}
    break;

  case 127:
#line 1201 "indra.y"
    {  
		(yyval.statement) = new LLScriptStatementSequence(gLine, gColumn, (yyvsp[(1) - (2)].statement), (yyvsp[(2) - (2)].statement));
		gAllocationManager->addAllocation((yyval.statement));
	}
    break;

  case 128:
#line 1209 "indra.y"
    {  
		(yyval.statement) = new LLScriptNOOP(gLine, gColumn);
		gAllocationManager->addAllocation((yyval.statement));
	}
    break;

  case 129:
#line 1214 "indra.y"
    {  
		LLScriptIdentifier	*id = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(2) - (3)].sval));	
		gAllocationManager->addAllocation(id);
		(yyval.statement) = new LLScriptStateChange(gLine, gColumn, id);
		gAllocationManager->addAllocation((yyval.statement));
	}
    break;

  case 130:
#line 1221 "indra.y"
    {  
		LLScriptIdentifier	*id = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(2) - (3)].sval));	
		gAllocationManager->addAllocation(id);
		(yyval.statement) = new LLScriptStateChange(gLine, gColumn, id);
		gAllocationManager->addAllocation((yyval.statement));
	}
    break;

  case 131:
#line 1228 "indra.y"
    {  
		LLScriptIdentifier	*id = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(2) - (3)].sval));	
		gAllocationManager->addAllocation(id);
		(yyval.statement) = new LLScriptJump(gLine, gColumn, id);
		gAllocationManager->addAllocation((yyval.statement));
	}
    break;

  case 132:
#line 1235 "indra.y"
    {  
		LLScriptIdentifier	*id = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(2) - (3)].sval));	
		gAllocationManager->addAllocation(id);
		(yyval.statement) = new LLScriptLabel(gLine, gColumn, id);
		gAllocationManager->addAllocation((yyval.statement));
	}
    break;

  case 133:
#line 1242 "indra.y"
    {  
		(yyval.statement) = new LLScriptReturn(gLine, gColumn, (yyvsp[(2) - (3)].expression));
		gAllocationManager->addAllocation((yyval.statement));
	}
    break;

  case 134:
#line 1247 "indra.y"
    {  
		(yyval.statement) = new LLScriptReturn(gLine, gColumn, NULL);
		gAllocationManager->addAllocation((yyval.statement));
	}
    break;

  case 135:
#line 1252 "indra.y"
    {  
		(yyval.statement) = new LLScriptExpressionStatement(gLine, gColumn, (yyvsp[(1) - (2)].expression));
		gAllocationManager->addAllocation((yyval.statement));
	}
    break;

  case 136:
#line 1257 "indra.y"
    {  
		(yyval.statement) = (yyvsp[(1) - (2)].statement);
	}
    break;

  case 137:
#line 1261 "indra.y"
    { 
		(yyval.statement) = (yyvsp[(1) - (1)].statement);
	}
    break;

  case 138:
#line 1265 "indra.y"
    {  
		(yyval.statement) = new LLScriptIf(gLine, gColumn, (yyvsp[(3) - (5)].expression), (yyvsp[(5) - (5)].statement));
		(yyvsp[(5) - (5)].statement)->mAllowDeclarations = FALSE;
		gAllocationManager->addAllocation((yyval.statement));
	}
    break;

  case 139:
#line 1271 "indra.y"
    {  
		(yyval.statement) = new LLScriptIfElse(gLine, gColumn, (yyvsp[(3) - (7)].expression), (yyvsp[(5) - (7)].statement), (yyvsp[(7) - (7)].statement));
		(yyvsp[(5) - (7)].statement)->mAllowDeclarations = FALSE;
		(yyvsp[(7) - (7)].statement)->mAllowDeclarations = FALSE;
		gAllocationManager->addAllocation((yyval.statement));
	}
    break;

  case 140:
#line 1278 "indra.y"
    {  
		(yyval.statement) = new LLScriptFor(gLine, gColumn, (yyvsp[(3) - (9)].expression), (yyvsp[(5) - (9)].expression), (yyvsp[(7) - (9)].expression), (yyvsp[(9) - (9)].statement));
		(yyvsp[(9) - (9)].statement)->mAllowDeclarations = FALSE;
		gAllocationManager->addAllocation((yyval.statement));
	}
    break;

  case 141:
#line 1284 "indra.y"
    {  
		(yyval.statement) = new LLScriptDoWhile(gLine, gColumn, (yyvsp[(2) - (7)].statement), (yyvsp[(5) - (7)].expression));
		(yyvsp[(2) - (7)].statement)->mAllowDeclarations = FALSE;
		gAllocationManager->addAllocation((yyval.statement));
	}
    break;

  case 142:
#line 1290 "indra.y"
    {  
		(yyval.statement) = new LLScriptWhile(gLine, gColumn, (yyvsp[(3) - (5)].expression), (yyvsp[(5) - (5)].statement));
		(yyvsp[(5) - (5)].statement)->mAllowDeclarations = FALSE;
		gAllocationManager->addAllocation((yyval.statement));
	}
    break;

  case 143:
#line 1299 "indra.y"
    {  
		LLScriptIdentifier	*id = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(2) - (2)].sval));	
		gAllocationManager->addAllocation(id);
		(yyval.statement) = new LLScriptDeclaration(gLine, gColumn, (yyvsp[(1) - (2)].type), id, NULL);
		gAllocationManager->addAllocation((yyval.statement));
	}
    break;

  case 144:
#line 1306 "indra.y"
    {  
		LLScriptIdentifier	*id = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(2) - (4)].sval));	
		gAllocationManager->addAllocation(id);
		(yyval.statement) = new LLScriptDeclaration(gLine, gColumn, (yyvsp[(1) - (4)].type), id, (yyvsp[(4) - (4)].expression));
		gAllocationManager->addAllocation((yyval.statement));
	}
    break;

  case 145:
#line 1316 "indra.y"
    {  
		(yyval.expression) = NULL;
	}
    break;

  case 146:
#line 1320 "indra.y"
    {
		(yyval.expression) = (yyvsp[(1) - (1)].expression);
	}
    break;

  case 147:
#line 1327 "indra.y"
    {  
		(yyval.expression) = new LLScriptForExpressionList(gLine, gColumn, (yyvsp[(1) - (1)].expression), NULL);
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 148:
#line 1332 "indra.y"
    {
		(yyval.expression) = new LLScriptForExpressionList(gLine, gColumn, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 149:
#line 1340 "indra.y"
    {  
		(yyval.expression) = NULL;
	}
    break;

  case 150:
#line 1344 "indra.y"
    {
		(yyval.expression) = (yyvsp[(1) - (1)].expression);
	}
    break;

  case 151:
#line 1351 "indra.y"
    {  
		(yyval.expression) = new LLScriptFuncExpressionList(gLine, gColumn, (yyvsp[(1) - (1)].expression), NULL);
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 152:
#line 1356 "indra.y"
    {
		(yyval.expression) = new LLScriptFuncExpressionList(gLine, gColumn, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 153:
#line 1364 "indra.y"
    {  
		(yyval.expression) = NULL;
	}
    break;

  case 154:
#line 1368 "indra.y"
    {
		(yyval.expression) = (yyvsp[(1) - (1)].expression);
	}
    break;

  case 155:
#line 1375 "indra.y"
    {  
		(yyval.expression) = new LLScriptListExpressionList(gLine, gColumn, (yyvsp[(1) - (1)].expression), NULL);
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 156:
#line 1380 "indra.y"
    {
		(yyval.expression) = new LLScriptListExpressionList(gLine, gColumn, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 157:
#line 1388 "indra.y"
    {  
		(yyval.expression) = (yyvsp[(1) - (1)].expression);
	}
    break;

  case 158:
#line 1392 "indra.y"
    {  
		(yyval.expression) = new LLScriptAssignment(gLine, gColumn, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 159:
#line 1397 "indra.y"
    {  
		(yyval.expression) = new LLScriptAddAssignment(gLine, gColumn, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 160:
#line 1402 "indra.y"
    {  
		(yyval.expression) = new LLScriptSubAssignment(gLine, gColumn, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 161:
#line 1407 "indra.y"
    {  
		(yyval.expression) = new LLScriptMulAssignment(gLine, gColumn, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 162:
#line 1412 "indra.y"
    {  
		(yyval.expression) = new LLScriptDivAssignment(gLine, gColumn, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 163:
#line 1417 "indra.y"
    {  
		(yyval.expression) = new LLScriptModAssignment(gLine, gColumn, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 164:
#line 1422 "indra.y"
    {  
		(yyval.expression) = new LLScriptEquality(gLine, gColumn, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 165:
#line 1427 "indra.y"
    {  
		(yyval.expression) = new LLScriptNotEquals(gLine, gColumn, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 166:
#line 1432 "indra.y"
    {  
		(yyval.expression) = new LLScriptLessEquals(gLine, gColumn, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 167:
#line 1437 "indra.y"
    {  
		(yyval.expression) = new LLScriptGreaterEquals(gLine, gColumn, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 168:
#line 1442 "indra.y"
    {  
		(yyval.expression) = new LLScriptLessThan(gLine, gColumn, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 169:
#line 1447 "indra.y"
    {  
		(yyval.expression) = new LLScriptGreaterThan(gLine, gColumn, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 170:
#line 1452 "indra.y"
    {  
		(yyval.expression) = new LLScriptPlus(gLine, gColumn, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 171:
#line 1457 "indra.y"
    {  
		(yyval.expression) = new LLScriptMinus(gLine, gColumn, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 172:
#line 1462 "indra.y"
    {  
		(yyval.expression) = new LLScriptTimes(gLine, gColumn, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 173:
#line 1467 "indra.y"
    {  
		(yyval.expression) = new LLScriptDivide(gLine, gColumn, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 174:
#line 1472 "indra.y"
    {  
		(yyval.expression) = new LLScriptMod(gLine, gColumn, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 175:
#line 1477 "indra.y"
    {  
		(yyval.expression) = new LLScriptBitAnd(gLine, gColumn, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 176:
#line 1482 "indra.y"
    {  
		(yyval.expression) = new LLScriptBitOr(gLine, gColumn, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 177:
#line 1487 "indra.y"
    {  
		(yyval.expression) = new LLScriptBitXor(gLine, gColumn, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 178:
#line 1492 "indra.y"
    {  
		(yyval.expression) = new LLScriptBooleanAnd(gLine, gColumn, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 179:
#line 1497 "indra.y"
    {  
		(yyval.expression) = new LLScriptBooleanOr(gLine, gColumn, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 180:
#line 1502 "indra.y"
    {
		(yyval.expression) = new LLScriptShiftLeft(gLine, gColumn, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 181:
#line 1507 "indra.y"
    {
		(yyval.expression) = new LLScriptShiftRight(gLine, gColumn, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 182:
#line 1515 "indra.y"
    {  
		(yyval.expression) = new LLScriptUnaryMinus(gLine, gColumn, (yyvsp[(2) - (2)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 183:
#line 1520 "indra.y"
    {  
		(yyval.expression) = new LLScriptBooleanNot(gLine, gColumn, (yyvsp[(2) - (2)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 184:
#line 1525 "indra.y"
    {  
		(yyval.expression) = new LLScriptBitNot(gLine, gColumn, (yyvsp[(2) - (2)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 185:
#line 1530 "indra.y"
    {  
		(yyval.expression) = new LLScriptPreIncrement(gLine, gColumn, (yyvsp[(2) - (2)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 186:
#line 1535 "indra.y"
    {  
		(yyval.expression) = new LLScriptPreDecrement(gLine, gColumn, (yyvsp[(2) - (2)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 187:
#line 1540 "indra.y"
    {
		(yyval.expression) = (yyvsp[(1) - (1)].expression);
	}
    break;

  case 188:
#line 1544 "indra.y"
    {  
		(yyval.expression) = (yyvsp[(1) - (1)].expression);
	}
    break;

  case 189:
#line 1548 "indra.y"
    {  
		(yyval.expression) = new LLScriptParenthesis(gLine, gColumn, (yyvsp[(2) - (3)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 190:
#line 1556 "indra.y"
    {
		(yyval.expression) = new LLScriptTypeCast(gLine, gColumn, (yyvsp[(2) - (4)].type), (yyvsp[(4) - (4)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 191:
#line 1561 "indra.y"
    {
		LLScriptConstantExpression *temp =  new LLScriptConstantExpression(gLine, gColumn, (yyvsp[(4) - (4)].constant));
		gAllocationManager->addAllocation(temp);
		(yyval.expression) = new LLScriptTypeCast(gLine, gColumn, (yyvsp[(2) - (4)].type), temp);
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 192:
#line 1568 "indra.y"
    {
		(yyval.expression) = new LLScriptTypeCast(gLine, gColumn, (yyvsp[(2) - (4)].type), (yyvsp[(4) - (4)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 193:
#line 1573 "indra.y"
    {
		(yyval.expression) = new LLScriptTypeCast(gLine, gColumn, (yyvsp[(2) - (6)].type), (yyvsp[(5) - (6)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 194:
#line 1581 "indra.y"
    {  
		(yyval.expression) = (yyvsp[(1) - (1)].expression);
	}
    break;

  case 195:
#line 1585 "indra.y"
    {
		(yyval.expression) = (yyvsp[(1) - (1)].expression);
	}
    break;

  case 196:
#line 1589 "indra.y"
    {  
		(yyval.expression) = (yyvsp[(1) - (1)].expression);
	}
    break;

  case 197:
#line 1593 "indra.y"
    {  
		(yyval.expression) = (yyvsp[(1) - (1)].expression);
	}
    break;

  case 198:
#line 1597 "indra.y"
    {  
		(yyval.expression) = new LLScriptPostIncrement(gLine, gColumn, (yyvsp[(1) - (2)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 199:
#line 1602 "indra.y"
    {  
		(yyval.expression) = new LLScriptPostDecrement(gLine, gColumn, (yyvsp[(1) - (2)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 200:
#line 1607 "indra.y"
    {  
		LLScriptIdentifier	*id = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(1) - (4)].sval));	
		gAllocationManager->addAllocation(id);
		(yyval.expression) = new LLScriptFunctionCall(gLine, gColumn, id, (yyvsp[(3) - (4)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 201:
#line 1614 "indra.y"
    {  
		(yyval.expression) = new LLScriptPrint(gLine, gColumn, (yyvsp[(3) - (4)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 202:
#line 1619 "indra.y"
    {  
		(yyval.expression) = new LLScriptConstantExpression(gLine, gColumn, (yyvsp[(1) - (1)].constant));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 203:
#line 1627 "indra.y"
    {
		(yyval.expression) = new LLScriptVectorInitializer(gLine, gColumn, (yyvsp[(2) - (7)].expression), (yyvsp[(4) - (7)].expression), (yyvsp[(6) - (7)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 204:
#line 1632 "indra.y"
    {
		LLScriptConstantFloat *cf0 = new LLScriptConstantFloat(gLine, gColumn, 0.f);
		gAllocationManager->addAllocation(cf0);
		LLScriptConstantExpression *sa0 = new LLScriptConstantExpression(gLine, gColumn, cf0);
		gAllocationManager->addAllocation(sa0);
		LLScriptConstantFloat *cf1 = new LLScriptConstantFloat(gLine, gColumn, 0.f);
		gAllocationManager->addAllocation(cf1);
		LLScriptConstantExpression *sa1 = new LLScriptConstantExpression(gLine, gColumn, cf1);
		gAllocationManager->addAllocation(sa1);
		LLScriptConstantFloat *cf2 = new LLScriptConstantFloat(gLine, gColumn, 0.f);
		gAllocationManager->addAllocation(cf2);
		LLScriptConstantExpression *sa2 = new LLScriptConstantExpression(gLine, gColumn, cf2);
		gAllocationManager->addAllocation(sa2);
		(yyval.expression) = new LLScriptVectorInitializer(gLine, gColumn, sa0, sa1, sa2);
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 205:
#line 1652 "indra.y"
    {
		(yyval.expression) = new LLScriptQuaternionInitializer(gLine, gColumn, (yyvsp[(2) - (9)].expression), (yyvsp[(4) - (9)].expression), (yyvsp[(6) - (9)].expression), (yyvsp[(8) - (9)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 206:
#line 1657 "indra.y"
    {
		LLScriptConstantFloat *cf0 = new LLScriptConstantFloat(gLine, gColumn, 0.f);
		gAllocationManager->addAllocation(cf0);
		LLScriptConstantExpression *sa0 = new LLScriptConstantExpression(gLine, gColumn, cf0);
		gAllocationManager->addAllocation(sa0);
		LLScriptConstantFloat *cf1 = new LLScriptConstantFloat(gLine, gColumn, 0.f);
		gAllocationManager->addAllocation(cf1);
		LLScriptConstantExpression *sa1 = new LLScriptConstantExpression(gLine, gColumn, cf1);
		gAllocationManager->addAllocation(sa1);
		LLScriptConstantFloat *cf2 = new LLScriptConstantFloat(gLine, gColumn, 0.f);
		gAllocationManager->addAllocation(cf2);
		LLScriptConstantExpression *sa2 = new LLScriptConstantExpression(gLine, gColumn, cf2);
		gAllocationManager->addAllocation(sa2);
		LLScriptConstantFloat *cf3 = new LLScriptConstantFloat(gLine, gColumn, 1.f);
		gAllocationManager->addAllocation(cf3);
		LLScriptConstantExpression *sa3 = new LLScriptConstantExpression(gLine, gColumn, cf3);
		gAllocationManager->addAllocation(sa3);
		(yyval.expression) = new LLScriptQuaternionInitializer(gLine, gColumn, sa0, sa1, sa2, sa3);
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 207:
#line 1681 "indra.y"
    {  
		(yyval.expression) = new LLScriptListInitializer(gLine, gColumn, (yyvsp[(2) - (3)].expression));
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 208:
#line 1689 "indra.y"
    {  
		LLScriptIdentifier	*id = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(1) - (1)].sval));	
		gAllocationManager->addAllocation(id);
		(yyval.expression) = new LLScriptLValue(gLine, gColumn, id, NULL);
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;

  case 209:
#line 1696 "indra.y"
    {
		LLScriptIdentifier	*id = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(1) - (3)].sval));	
		gAllocationManager->addAllocation(id);
		LLScriptIdentifier	*ac = new LLScriptIdentifier(gLine, gColumn, (yyvsp[(3) - (3)].sval));	
		gAllocationManager->addAllocation(id);
		(yyval.expression) = new LLScriptLValue(gLine, gColumn, id, ac);
		gAllocationManager->addAllocation((yyval.expression));
	}
    break;


/* Line 1267 of yacc.c.  */
#line 3992 "ytab.cpp"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 1706 "indra.y"


