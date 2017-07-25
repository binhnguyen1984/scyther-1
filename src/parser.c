/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 20 "parser.y" /* yacc.c:339  */

#include "pheading.h"
#include "tac.h"
#include "error.h"
#include "list.h"
#include "string.h"
#include "switches.h"

struct tacnode*	spdltac;

int yyerror(char *s);
int yylex(void);

List macrolist=NULL;

List findMacroDefinition(Symbol s)
{
	List l;

	if (s == NULL)
	  {
	     return NULL;
	  }
	/* Now check if it is in the list
	 * already */
	for (l = list_rewind(macrolist); l != NULL; l = l->next)
	  {
	    Tac tmacro;
	    Symbol sl;
		
	    tmacro = (Tac) l->data;
	    sl = (tmacro->t1.tac)->t1.sym;	// The symbol
	    if (strcmp (sl->text, s->text) == 0)
	      {
	        return l;
	      }
	  }
	return NULL;
}


#line 108 "parser.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "parser.h".  */
#ifndef YY_YY_PARSER_H_INCLUDED
# define YY_YY_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    ID = 258,
    TEXT = 259,
    PROTOCOL = 260,
    ROLE = 261,
    READT = 262,
    RECVT = 263,
    SENDT = 264,
    CLAIMT = 265,
    VAR = 266,
    CONST = 267,
    FRESH = 268,
    RUN = 269,
    SECRET = 270,
    COMPROMISED = 271,
    INVERSEKEYS = 272,
    INVERSEKEYFUNCTIONS = 273,
    UNTRUSTED = 274,
    USERTYPE = 275,
    SINGULAR = 276,
    FUNCTION = 277,
    HASHFUNCTION = 278,
    KNOWS = 279,
    TRUSTED = 280,
    OPTION = 281,
    MACRO = 282,
    MATCH = 283,
    NOT = 284,
    SUBTYPING = 285,
    ENV = 286,
    EQUATIONS = 287,
    ESSENTIAL = 288
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 62 "parser.y" /* yacc.c:355  */

	char*  		str;
	struct tacnode*	tac;
	Symbol		symb;
	int		value;

#line 189 "parser.c" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_PARSER_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 206 "parser.c" /* yacc.c:358  */

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
#else
typedef signed char yytype_int8;
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
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
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
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  48
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   223

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  48
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  35
/* YYNRULES -- Number of rules.  */
#define YYNRULES  84
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  221

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   288

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      35,    36,     2,     2,    41,     2,    46,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    39,    34,
      40,    45,     2,     2,    44,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    42,     2,    43,     2,    47,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    37,     2,    38,     2,     2,     2,     2,
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
      25,    26,    27,    28,    29,    30,    31,    32,    33
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   142,   142,   147,   148,   152,   158,   165,   173,   179,
     185,   192,   199,   203,   208,   215,   216,   220,   222,   226,
     233,   237,   241,   247,   249,   257,   266,   269,   275,   284,
     287,   295,   304,   305,   307,   311,   323,   324,   329,   330,
     335,   336,   338,   340,   348,   355,   362,   369,   379,   389,
     398,   406,   413,   446,   453,   459,   466,   472,   478,   484,
     489,   495,   502,   508,   510,   515,   518,   526,   530,   537,
     541,   547,   552,   553,   558,   568,   592,   598,   602,   606,
     610,   612,   616,   618,   622
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ID", "TEXT", "PROTOCOL", "ROLE",
  "READT", "RECVT", "SENDT", "CLAIMT", "VAR", "CONST", "FRESH", "RUN",
  "SECRET", "COMPROMISED", "INVERSEKEYS", "INVERSEKEYFUNCTIONS",
  "UNTRUSTED", "USERTYPE", "SINGULAR", "FUNCTION", "HASHFUNCTION", "KNOWS",
  "TRUSTED", "OPTION", "MACRO", "MATCH", "NOT", "SUBTYPING", "ENV",
  "EQUATIONS", "ESSENTIAL", "';'", "'('", "')'", "'{'", "'}'", "':'",
  "'<'", "','", "'['", "']'", "'@'", "'='", "'.'", "'_'", "$accept",
  "spdlcomplete", "spdlrep", "spdl", "subtyping", "subtypinglist",
  "patternlist", "pattern", "equation", "eqlist", "typedeclare",
  "typelist", "options", "roles", "role", "singular", "optclosing",
  "roledef", "event", "roleref", "knowsdecl", "macrodecl", "declaration",
  "funcORhash", "secretpref", "typeinfo1", "typeinfoN", "label",
  "optlabel", "basicterm", "basicormacro", "term", "termlist",
  "basictermlist", "key", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,    59,    40,    41,   123,   125,    58,
      60,    44,    91,    93,    64,    61,    46,    95
};
# endif

#define YYPACT_NINF -158

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-158)))

#define YYTABLE_NINF -41

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     122,     7,    18,    23,    18,    37,    -3,    -2,    37,    37,
      30,    18,    34,    -1,     0,    18,    41,  -158,   122,  -158,
    -158,  -158,     5,     9,  -158,    14,    17,    13,    27,    17,
      28,    37,    37,  -158,    32,    35,    37,    37,    47,    50,
      52,    42,    49,    53,    54,    18,    48,    59,  -158,  -158,
      18,    18,  -158,  -158,    18,    37,    18,    18,    61,    72,
      37,    62,    37,    68,    67,    37,  -158,    69,    70,  -158,
    -158,  -158,  -158,    37,   105,    34,  -158,    18,  -158,    75,
      74,     8,     8,    37,     8,    71,    48,  -158,  -158,  -158,
      76,    17,    83,    84,  -158,  -158,  -158,  -158,    88,  -158,
      89,  -158,    37,  -158,    37,    37,    87,  -158,  -158,  -158,
      37,     8,    90,    78,    94,    91,    98,     8,  -158,    18,
     110,   111,  -158,   113,   123,  -158,  -158,  -158,    92,   120,
    -158,   125,   126,  -158,     8,     8,  -158,     8,   127,  -158,
    -158,  -158,    85,  -158,   131,   132,  -158,  -158,  -158,  -158,
     133,  -158,  -158,   109,    85,   162,    85,  -158,  -158,  -158,
      52,  -158,   166,  -158,  -158,   134,   165,   129,   129,   129,
     129,    37,   144,   160,   152,   165,   165,   165,   188,   161,
     164,   167,  -158,   169,   163,    37,   170,    52,  -158,  -158,
    -158,  -158,    37,    37,    37,    37,  -158,   154,    37,  -158,
     171,   172,   173,   174,    37,   159,   177,   178,   179,   180,
     181,    37,  -158,  -158,  -158,  -158,   182,   183,  -158,   184,
    -158
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
      65,     0,     0,     0,    66,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     2,    65,    13,
      62,    12,     0,     0,    74,    82,    67,     0,     0,    67,
      75,     0,     0,    79,    80,     0,     0,     0,     0,     0,
      38,     0,     0,    15,     0,    29,    26,     0,     1,     4,
       0,     0,    63,    64,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    59,     0,     0,     5,
       8,    39,    31,     0,     0,     0,     9,    29,    10,     0,
      74,     0,     0,     0,     0,     0,    26,    11,    23,    60,
      69,    67,     0,     0,    83,    68,    54,    50,     0,    56,
       0,    78,     0,    81,     0,     0,     0,    14,    16,    30,
       0,     0,     0,    17,     0,     0,     0,     0,    27,     0,
       0,     0,    61,     0,     0,    76,    84,    77,     0,     0,
      52,     0,     0,    21,     0,     0,    22,     0,     0,    70,
      55,    53,    65,     6,     0,     0,    28,    19,    18,    20,
       0,    25,    37,     0,    65,     0,    65,    57,    58,    24,
      38,    33,     0,    34,     7,     0,    65,     0,     0,     0,
      72,     0,     0,     0,     0,    65,    65,    65,     0,     0,
       0,     0,    73,     0,     0,     0,     0,    38,    41,    43,
      42,    71,     0,     0,     0,     0,    51,     0,     0,    35,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    44,    45,    46,    49,     0,     0,    47,     0,
      48
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -158,  -158,   197,  -158,  -158,   145,   -76,   -38,  -158,   115,
    -158,   146,  -158,  -141,  -158,  -158,  -151,  -157,  -158,  -158,
    -158,  -158,     4,  -158,  -158,   -24,  -158,  -139,  -158,    -4,
    -158,   -34,    -8,    10,  -158
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    16,    17,    18,    43,    44,   112,   113,    86,    87,
      77,    78,    19,   153,   154,   155,    72,   174,   175,    28,
     176,    20,   177,    54,    22,    58,   120,   179,   183,    25,
      33,    34,    35,    79,   127
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      38,    39,    67,    68,    21,    61,   114,    41,    85,   164,
      23,    80,    26,   161,    29,   163,    50,    51,   188,   189,
     190,    24,    21,    63,    64,    47,    27,    52,    53,   180,
     181,   182,    36,    37,    40,   132,   199,    42,    45,    46,
      30,    48,    88,    81,    55,    82,   116,    93,    85,   115,
      83,    80,    98,    95,   100,    56,    57,   103,   148,    59,
      90,    91,    60,    62,    92,   106,    94,   121,   126,    66,
     128,   129,    31,    65,    32,    97,   131,    88,    88,   138,
      88,    69,    88,    81,    70,    82,    71,    73,    76,    74,
      83,   -36,    84,    89,    75,    96,    99,   149,     2,   150,
       4,     5,     6,     7,   101,   102,   152,    88,   107,   111,
     104,   105,    11,    88,   110,   119,   117,   122,    15,   134,
     123,   130,    -3,   -32,   124,   125,   133,     1,   144,   139,
      88,    88,   135,    88,   136,     2,     3,     4,     5,     6,
       7,     8,     9,   137,   140,   141,   156,   160,    10,    11,
     142,   197,    12,    13,    14,    15,   145,   143,   156,   146,
     156,   151,   147,   184,   205,   157,   158,   159,   162,   165,
     210,   166,   167,   168,   169,   170,   178,   217,     2,   185,
       4,     5,     6,     7,   200,   201,   202,   203,   186,   171,
     187,   191,    11,   172,   173,   204,   192,   196,    15,   193,
     211,   118,   194,   -40,   195,   198,     0,   206,   207,   208,
     209,   212,   213,   214,   215,    49,   218,   216,   220,   219,
     108,     0,     0,   109
};

static const yytype_int16 yycheck[] =
{
       8,     9,    36,    37,     0,    29,    82,    11,    46,   160,
       3,     3,     2,   154,     4,   156,    11,    12,   175,   176,
     177,     3,    18,    31,    32,    15,     3,    22,    23,   168,
     169,   170,    35,    35,     4,   111,   187,     3,    39,    39,
       3,     0,    46,    35,    35,    37,    84,    55,    86,    83,
      42,     3,    60,    57,    62,    41,    39,    65,   134,    46,
      50,    51,    35,    35,    54,    73,    56,    91,   102,    34,
     104,   105,    35,    41,    37,     3,   110,    81,    82,   117,
      84,    34,    86,    35,    34,    37,    34,    45,    34,    40,
      42,     6,    44,    34,    41,    34,    34,   135,    13,   137,
      15,    16,    17,    18,    36,    38,    21,   111,     3,    35,
      41,    41,    27,   117,    39,    39,    45,    34,    33,    41,
      36,    34,     0,    38,    36,    36,    36,     5,    36,   119,
     134,   135,    38,   137,    43,    13,    14,    15,    16,    17,
      18,    19,    20,    45,    34,    34,   142,    38,    26,    27,
      37,   185,    30,    31,    32,    33,    36,    34,   154,    34,
     156,    34,    36,   171,   198,    34,    34,    34,     6,     3,
     204,    37,     7,     8,     9,    10,    47,   211,    13,    35,
      15,    16,    17,    18,   192,   193,   194,   195,    28,    24,
      38,     3,    27,    28,    29,    41,    35,    34,    33,    35,
      41,    86,    35,    38,    35,    35,    -1,    36,    36,    36,
      36,    34,    34,    34,    34,    18,    34,    36,    34,    36,
      75,    -1,    -1,    77
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     5,    13,    14,    15,    16,    17,    18,    19,    20,
      26,    27,    30,    31,    32,    33,    49,    50,    51,    60,
      69,    70,    72,     3,     3,    77,    81,     3,    67,    81,
       3,    35,    37,    78,    79,    80,    35,    35,    80,    80,
       4,    77,     3,    52,    53,    39,    39,    81,     0,    50,
      11,    12,    22,    23,    71,    35,    41,    39,    73,    46,
      35,    73,    35,    80,    80,    41,    34,    79,    79,    34,
      34,    34,    64,    45,    40,    41,    34,    58,    59,    81,
       3,    35,    37,    42,    44,    55,    56,    57,    77,    34,
      81,    81,    81,    80,    81,    77,    34,     3,    80,    34,
      80,    36,    38,    80,    41,    41,    80,     3,    53,    59,
      39,    35,    54,    55,    54,    79,    55,    45,    57,    39,
      74,    73,    34,    36,    36,    36,    79,    82,    79,    79,
      34,    79,    54,    36,    41,    38,    43,    45,    55,    81,
      34,    34,    37,    34,    36,    36,    34,    36,    54,    55,
      55,    34,    21,    61,    62,    63,    70,    34,    34,    34,
      38,    61,     6,    61,    64,     3,    37,     7,     8,     9,
      10,    24,    28,    29,    65,    66,    68,    70,    47,    75,
      75,    75,    75,    76,    80,    35,    28,    38,    65,    65,
      65,     3,    35,    35,    35,    35,    34,    79,    35,    64,
      80,    80,    80,    80,    41,    79,    36,    36,    36,    36,
      79,    41,    34,    34,    34,    34,    36,    79,    34,    36,
      34
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    48,    49,    50,    50,    51,    51,    51,    51,    51,
      51,    51,    51,    51,    52,    53,    53,    54,    54,    55,
      55,    55,    55,    55,    56,    56,    57,    57,    58,    59,
      59,    60,    61,    61,    61,    62,    63,    63,    64,    64,
      65,    65,    65,    65,    66,    66,    66,    66,    66,    66,
      67,    68,    69,    70,    70,    70,    70,    70,    70,    70,
      70,    70,    70,    71,    71,    72,    72,    73,    73,    74,
      74,    75,    76,    76,    77,    78,    79,    79,    79,    79,
      80,    80,    81,    81,    82
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     3,     6,     9,     3,     3,
       3,     3,     1,     1,     3,     1,     3,     1,     3,     4,
       4,     3,     3,     1,     5,     4,     0,     2,     4,     0,
       2,     3,     0,     2,     2,     7,     0,     1,     0,     1,
       0,     2,     2,     2,     6,     6,     6,     7,     8,     6,
       3,     3,     5,     5,     4,     5,     4,     7,     7,     3,
       3,     4,     1,     1,     1,     0,     1,     0,     2,     0,
       2,     2,     0,     1,     1,     1,     4,     4,     3,     1,
       1,     3,     1,     3,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

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
#ifndef YYINITDEPTH
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
static YYSIZE_T
yystrlen (const char *yystr)
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
static char *
yystpcpy (char *yydest, const char *yysrc)
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

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
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
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
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

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
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
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

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
     '$$ = $1'.

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
#line 143 "parser.y" /* yacc.c:1646  */
    {	spdltac = (yyvsp[0].tac); }
#line 1430 "parser.c" /* yacc.c:1646  */
    break;

  case 3:
#line 147 "parser.y" /* yacc.c:1646  */
    {	(yyval.tac) = NULL; }
#line 1436 "parser.c" /* yacc.c:1646  */
    break;

  case 4:
#line 149 "parser.y" /* yacc.c:1646  */
    {	(yyval.tac) = tacCat((yyvsp[-1].tac),(yyvsp[0].tac)); }
#line 1442 "parser.c" /* yacc.c:1646  */
    break;

  case 5:
#line 153 "parser.y" /* yacc.c:1646  */
    {	
		  	Tac t = tacCreate(TAC_UNTRUSTED);
		  	t->t1.tac = (yyvsp[-1].tac);
			(yyval.tac) = t;
		  }
#line 1452 "parser.c" /* yacc.c:1646  */
    break;

  case 6:
#line 159 "parser.y" /* yacc.c:1646  */
    {	
		  	Tac t = tacCreate(TAC_RUN);
		  	t->t1.tac = (yyvsp[-4].tac);
			t->t2.tac = (yyvsp[-2].tac);
			(yyval.tac) = t;
		  }
#line 1463 "parser.c" /* yacc.c:1646  */
    break;

  case 7:
#line 166 "parser.y" /* yacc.c:1646  */
    {
		  	Tac t = tacCreate(TAC_PROTOCOL);
			t->t1.sym = (yyvsp[-7].symb);
			t->t2.tac = (yyvsp[-2].tac);
			t->t3.tac = (yyvsp[-5].tac);
			(yyval.tac) = t;
		  }
#line 1475 "parser.c" /* yacc.c:1646  */
    break;

  case 8:
#line 174 "parser.y" /* yacc.c:1646  */
    {
			Tac t = tacCreate(TAC_USERTYPE);
			t->t1.tac = (yyvsp[-1].tac);
			(yyval.tac) = t;
		  }
#line 1485 "parser.c" /* yacc.c:1646  */
    break;

  case 9:
#line 180 "parser.y" /* yacc.c:1646  */
    {
			Tac t = tacCreate(TAC_USER_SUBTYPE);
			t->t1.tac = (yyvsp[-1].tac);
			(yyval.tac) = t;			
			}
#line 1495 "parser.c" /* yacc.c:1646  */
    break;

  case 10:
#line 186 "parser.y" /* yacc.c:1646  */
    {
				Tac t = tacCreate(TAC_ENV);
				t->t1.tac = (yyvsp[0].tac);
				(yyval.tac) = t;
			}
#line 1505 "parser.c" /* yacc.c:1646  */
    break;

  case 11:
#line 193 "parser.y" /* yacc.c:1646  */
    {
				Tac t = tacCreate(TAC_EQUATIONS);
				t->t1.tac = (yyvsp[0].tac);
				(yyval.tac) = t;
			}
#line 1515 "parser.c" /* yacc.c:1646  */
    break;

  case 12:
#line 200 "parser.y" /* yacc.c:1646  */
    {
		  	(yyval.tac) = (yyvsp[0].tac);
		  }
#line 1523 "parser.c" /* yacc.c:1646  */
    break;

  case 13:
#line 204 "parser.y" /* yacc.c:1646  */
    {
		  	(yyval.tac) = (yyvsp[0].tac);
		  }
#line 1531 "parser.c" /* yacc.c:1646  */
    break;

  case 14:
#line 209 "parser.y" /* yacc.c:1646  */
    {
				Tac t = tacCreate(TAC_SUBTYPE);
				t->t1.sym=(yyvsp[-2].symb);
				t->t2.sym=(yyvsp[0].symb);
				(yyval.tac)=t;
			}
#line 1542 "parser.c" /* yacc.c:1646  */
    break;

  case 15:
#line 215 "parser.y" /* yacc.c:1646  */
    {}
#line 1548 "parser.c" /* yacc.c:1646  */
    break;

  case 16:
#line 217 "parser.y" /* yacc.c:1646  */
    {(yyval.tac) = tacCat((yyvsp[-2].tac),(yyvsp[0].tac));}
#line 1554 "parser.c" /* yacc.c:1646  */
    break;

  case 17:
#line 221 "parser.y" /* yacc.c:1646  */
    {}
#line 1560 "parser.c" /* yacc.c:1646  */
    break;

  case 18:
#line 223 "parser.y" /* yacc.c:1646  */
    {	(yyval.tac) = tacCat((yyvsp[-2].tac),(yyvsp[0].tac)); }
#line 1566 "parser.c" /* yacc.c:1646  */
    break;

  case 19:
#line 227 "parser.y" /* yacc.c:1646  */
    {
			  	Tac t = tacCreate(TAC_STRING);
				t->t1.sym = (yyvsp[-3].symb);
				(yyval.tac) = tacJoin(TAC_FCALL,tacTuple((yyvsp[-1].tac)),t,NULL);
			}
#line 1576 "parser.c" /* yacc.c:1646  */
    break;

  case 20:
#line 234 "parser.y" /* yacc.c:1646  */
    {
		  	(yyval.tac) = tacJoin(TAC_ENCRYPT,tacTuple((yyvsp[-2].tac)),(yyvsp[0].tac),NULL);
		  }
#line 1584 "parser.c" /* yacc.c:1646  */
    break;

  case 21:
#line 238 "parser.y" /* yacc.c:1646  */
    { 
		  	(yyval.tac) = tacTuple((yyvsp[-1].tac));
		  }
#line 1592 "parser.c" /* yacc.c:1646  */
    break;

  case 22:
#line 242 "parser.y" /* yacc.c:1646  */
    {
				Tac t = tacCreate(TAC_ABSTRACT);
				t->t1.tac = (yyvsp[-1].tac);
				(yyval.tac) = t;
			}
#line 1602 "parser.c" /* yacc.c:1646  */
    break;

  case 23:
#line 247 "parser.y" /* yacc.c:1646  */
    {}
#line 1608 "parser.c" /* yacc.c:1646  */
    break;

  case 24:
#line 250 "parser.y" /* yacc.c:1646  */
    {
				Tac t= tacCreate(TAC_PERSISTENT_EQUATION);
				t->t1.tac = (yyvsp[-3].tac);
				t->t2.tac = (yyvsp[-1].tac);
				(yyval.tac) = t;
			}
#line 1619 "parser.c" /* yacc.c:1646  */
    break;

  case 25:
#line 258 "parser.y" /* yacc.c:1646  */
    {
				Tac t = tacCreate(TAC_EQUATION);
				t->t1.tac = (yyvsp[-3].tac);
				t->t2.tac = (yyvsp[-1].tac);
				(yyval.tac) = t;
			}
#line 1630 "parser.c" /* yacc.c:1646  */
    break;

  case 26:
#line 266 "parser.y" /* yacc.c:1646  */
    {
				(yyval.tac)=NULL;
			}
#line 1638 "parser.c" /* yacc.c:1646  */
    break;

  case 27:
#line 270 "parser.y" /* yacc.c:1646  */
    {
				(yyval.tac) = tacCat((yyvsp[-1].tac),(yyvsp[0].tac));
			}
#line 1646 "parser.c" /* yacc.c:1646  */
    break;

  case 28:
#line 276 "parser.y" /* yacc.c:1646  */
    {
				Tac t = tacCreate(TAC_TYPING);
				t->t1.tac = (yyvsp[-3].tac);
				t->t2.tac = (yyvsp[-1].tac);
				(yyval.tac) = t;
			}
#line 1657 "parser.c" /* yacc.c:1646  */
    break;

  case 29:
#line 284 "parser.y" /* yacc.c:1646  */
    {
				(yyval.tac)=NULL;
			}
#line 1665 "parser.c" /* yacc.c:1646  */
    break;

  case 30:
#line 288 "parser.y" /* yacc.c:1646  */
    {
				(yyval.tac) = tacCat((yyvsp[-1].tac),(yyvsp[0].tac));
			}
#line 1673 "parser.c" /* yacc.c:1646  */
    break;

  case 31:
#line 296 "parser.y" /* yacc.c:1646  */
    {
		  	// Process 'option' as command-line options.
		  	process_switch_buffer((yyvsp[-1].str));
			(yyval.tac) = NULL;
		  }
#line 1683 "parser.c" /* yacc.c:1646  */
    break;

  case 32:
#line 304 "parser.y" /* yacc.c:1646  */
    {	(yyval.tac) = NULL; }
#line 1689 "parser.c" /* yacc.c:1646  */
    break;

  case 33:
#line 306 "parser.y" /* yacc.c:1646  */
    {	(yyval.tac) = tacCat((yyvsp[-1].tac),(yyvsp[0].tac)); }
#line 1695 "parser.c" /* yacc.c:1646  */
    break;

  case 34:
#line 308 "parser.y" /* yacc.c:1646  */
    {	(yyval.tac) = tacCat((yyvsp[-1].tac),(yyvsp[0].tac)); }
#line 1701 "parser.c" /* yacc.c:1646  */
    break;

  case 35:
#line 312 "parser.y" /* yacc.c:1646  */
    { 
		  	// TODO process singular (0/1)
		  	Tac t = tacCreate(TAC_ROLE);
			t->t1.sym = (yyvsp[-4].symb);
			t->t2.tac = (yyvsp[-2].tac);
			t->t3.value = (yyvsp[-6].value);
			(yyval.tac) = t;
		  }
#line 1714 "parser.c" /* yacc.c:1646  */
    break;

  case 36:
#line 323 "parser.y" /* yacc.c:1646  */
    {	(yyval.value) = 0; }
#line 1720 "parser.c" /* yacc.c:1646  */
    break;

  case 37:
#line 325 "parser.y" /* yacc.c:1646  */
    {	(yyval.value) = 1; }
#line 1726 "parser.c" /* yacc.c:1646  */
    break;

  case 38:
#line 329 "parser.y" /* yacc.c:1646  */
    { }
#line 1732 "parser.c" /* yacc.c:1646  */
    break;

  case 39:
#line 331 "parser.y" /* yacc.c:1646  */
    { }
#line 1738 "parser.c" /* yacc.c:1646  */
    break;

  case 40:
#line 335 "parser.y" /* yacc.c:1646  */
    {	(yyval.tac) = NULL; }
#line 1744 "parser.c" /* yacc.c:1646  */
    break;

  case 41:
#line 337 "parser.y" /* yacc.c:1646  */
    {	(yyval.tac) = tacCat((yyvsp[-1].tac),(yyvsp[0].tac)); }
#line 1750 "parser.c" /* yacc.c:1646  */
    break;

  case 42:
#line 339 "parser.y" /* yacc.c:1646  */
    {	(yyval.tac) = tacCat((yyvsp[-1].tac),(yyvsp[0].tac)); }
#line 1756 "parser.c" /* yacc.c:1646  */
    break;

  case 43:
#line 341 "parser.y" /* yacc.c:1646  */
    {	(yyval.tac) = tacCat((yyvsp[-1].tac),(yyvsp[0].tac)); }
#line 1762 "parser.c" /* yacc.c:1646  */
    break;

  case 44:
#line 349 "parser.y" /* yacc.c:1646  */
    {	Tac t = tacCreate(TAC_RECV);
		  	t->t1.sym = (yyvsp[-4].symb);
			/* TODO test here: tac2 should have at least 3 elements */
			t->t2.tac = (yyvsp[-2].tac);
			(yyval.tac) = t;
		  }
#line 1773 "parser.c" /* yacc.c:1646  */
    break;

  case 45:
#line 356 "parser.y" /* yacc.c:1646  */
    {	Tac t = tacCreate(TAC_RECV);
		  	t->t1.sym = (yyvsp[-4].symb);
			/* TODO test here: tac2 should have at least 3 elements */
			t->t2.tac = (yyvsp[-2].tac);
			(yyval.tac) = t;
		  }
#line 1784 "parser.c" /* yacc.c:1646  */
    break;

  case 46:
#line 363 "parser.y" /* yacc.c:1646  */
    {	Tac t = tacCreate(TAC_SEND);
		  	t->t1.sym = (yyvsp[-4].symb);
			/* TODO test here: tac2 should have at least 3 elements */
			t->t2.tac = (yyvsp[-2].tac);
			(yyval.tac) = t;
		  }
#line 1795 "parser.c" /* yacc.c:1646  */
    break;

  case 47:
#line 370 "parser.y" /* yacc.c:1646  */
    {
		  	/* first argument is pattern, second should be
			 * ground term */
		  	Tac t= tacCreate(TAC_MATCH);
			t->t1.tac = (yyvsp[-4].tac);
			t->t2.tac = (yyvsp[-2].tac);
			t->t3.value = true;
			(yyval.tac) = t;
		  }
#line 1809 "parser.c" /* yacc.c:1646  */
    break;

  case 48:
#line 380 "parser.y" /* yacc.c:1646  */
    {
		  	/* first argument is pattern, second should be
			 * ground term */
		  	Tac t= tacCreate(TAC_MATCH);
			t->t1.tac = (yyvsp[-4].tac);
			t->t2.tac = (yyvsp[-2].tac);
			t->t3.value = false;
			(yyval.tac) = t;
		  }
#line 1823 "parser.c" /* yacc.c:1646  */
    break;

  case 49:
#line 391 "parser.y" /* yacc.c:1646  */
    {	Tac t = tacCreate(TAC_CLAIM);
		  	t->t1.sym = (yyvsp[-4].symb);
			t->t2.tac = (yyvsp[-2].tac);
			(yyval.tac) = t;
		  }
#line 1833 "parser.c" /* yacc.c:1646  */
    break;

  case 50:
#line 399 "parser.y" /* yacc.c:1646  */
    {	Tac t = tacCreate(TAC_ROLEREF);
		  	t->t1.sym = (yyvsp[-2].symb);
			t->t2.sym = (yyvsp[0].symb);
			(yyval.tac) = t;
		  }
#line 1843 "parser.c" /* yacc.c:1646  */
    break;

  case 51:
#line 407 "parser.y" /* yacc.c:1646  */
    {	Tac t = tacCreate(TAC_KNOWS);
		  	t->t1.tac = (yyvsp[-1].tac);
			(yyval.tac) = t;
		  }
#line 1852 "parser.c" /* yacc.c:1646  */
    break;

  case 52:
#line 414 "parser.y" /* yacc.c:1646  */
    {	
			List l;
		        Tac t;

			l = findMacroDefinition((yyvsp[-3].tac)->t1.sym);
			if (l != NULL)
			  {
			    /* Already defined. We can consider this
			     * a bug or we can define it to
			     * overwrite.
			     *
			     * For now, we decide to overwrite. This
			     * will also aid in straightforward file
			     * composition, if both files contain
			     * macros.
			     */
			    macrolist = list_delete(l);
			    // Alternative:
			    //yyerror("macro symbol already defined earlier.");
			  }

			// Now we know that l does not occur in macrolist
		        /* Add to macro declaration list.
		         * TAC_MACRO doesn't show up in the compiler though.  */
		        t = tacCreate(TAC_MACRO);
		        t->t1.tac = (yyvsp[-3].tac);
		        t->t2.tac = tacTuple((yyvsp[-1].tac));
		        macrolist = list_append(macrolist, t);
			(yyval.tac) = NULL;
		  }
#line 1887 "parser.c" /* yacc.c:1646  */
    break;

  case 53:
#line 447 "parser.y" /* yacc.c:1646  */
    {	Tac t = tacCreate(TAC_CONST);
		  	t->t1.tac = (yyvsp[-2].tac); // names
			t->t2.tac = (yyvsp[-1].tac); // type
			t->t3.tac = (yyvsp[-4].tac); // secret?
			(yyval.tac) = t;
		  }
#line 1898 "parser.c" /* yacc.c:1646  */
    break;

  case 54:
#line 454 "parser.y" /* yacc.c:1646  */
    {	Tac t = tacCreate(TAC_FRESH);
		  	t->t1.tac = (yyvsp[-2].tac);	// names
			t->t2.tac = (yyvsp[-1].tac); // type
			(yyval.tac) = t;
		  }
#line 1908 "parser.c" /* yacc.c:1646  */
    break;

  case 55:
#line 460 "parser.y" /* yacc.c:1646  */
    {	Tac t = tacCreate(TAC_VAR);
		  	t->t1.tac = (yyvsp[-2].tac);
			t->t2.tac = (yyvsp[-1].tac);
			t->t3.tac = (yyvsp[-4].tac); // obsolete: should not even occur at the global level
			(yyval.tac) = t;
		  }
#line 1919 "parser.c" /* yacc.c:1646  */
    break;

  case 56:
#line 467 "parser.y" /* yacc.c:1646  */
    {	Tac t = tacCreate(TAC_SECRET);
		  	t->t1.tac = (yyvsp[-2].tac);
			t->t2.tac = (yyvsp[-1].tac);
			(yyval.tac) = t;
		  }
#line 1929 "parser.c" /* yacc.c:1646  */
    break;

  case 57:
#line 473 "parser.y" /* yacc.c:1646  */
    {	Tac t = tacCreate(TAC_INVERSEKEYS);
			t->t1.tac = (yyvsp[-4].tac);
			t->t2.tac = (yyvsp[-2].tac);
			(yyval.tac) = t;
		  }
#line 1939 "parser.c" /* yacc.c:1646  */
    break;

  case 58:
#line 479 "parser.y" /* yacc.c:1646  */
    {	Tac t = tacCreate(TAC_INVERSEKEYFUNCTIONS);
			t->t1.tac = (yyvsp[-4].tac);
			t->t2.tac = (yyvsp[-2].tac);
			(yyval.tac) = t;
		  }
#line 1949 "parser.c" /* yacc.c:1646  */
    break;

  case 59:
#line 485 "parser.y" /* yacc.c:1646  */
    {	Tac t = tacCreate(TAC_COMPROMISED);
		  	t->t1.tac= (yyvsp[-1].tac);
			(yyval.tac) = t;
		  }
#line 1958 "parser.c" /* yacc.c:1646  */
    break;

  case 60:
#line 490 "parser.y" /* yacc.c:1646  */
    {
				Tac t = tacCreate(TAC_ESSENTIAL);
				t->t1.tac = (yyvsp[-1].tac);
				(yyval.tac) = t;				
			}
#line 1968 "parser.c" /* yacc.c:1646  */
    break;

  case 61:
#line 496 "parser.y" /* yacc.c:1646  */
    {	Tac t = tacCreate(TAC_HASHFUNCTION);
		  	t->t1.tac = (yyvsp[-1].tac);
		  	t->t2.tac = tacCreate(TAC_UNDEF);
			t->t3.tac = (yyvsp[-3].tac);	// secret or not
			(yyval.tac) = t;
		  }
#line 1979 "parser.c" /* yacc.c:1646  */
    break;

  case 62:
#line 503 "parser.y" /* yacc.c:1646  */
    {	
		  	(yyval.tac) = (yyvsp[0].tac);
		  }
#line 1987 "parser.c" /* yacc.c:1646  */
    break;

  case 63:
#line 509 "parser.y" /* yacc.c:1646  */
    { }
#line 1993 "parser.c" /* yacc.c:1646  */
    break;

  case 64:
#line 511 "parser.y" /* yacc.c:1646  */
    { }
#line 1999 "parser.c" /* yacc.c:1646  */
    break;

  case 65:
#line 515 "parser.y" /* yacc.c:1646  */
    {	
			(yyval.tac) = NULL;
		  }
#line 2007 "parser.c" /* yacc.c:1646  */
    break;

  case 66:
#line 519 "parser.y" /* yacc.c:1646  */
    {
			Tac t = tacCreate(TAC_SECRET);
			(yyval.tac) = t;
		  }
#line 2016 "parser.c" /* yacc.c:1646  */
    break;

  case 67:
#line 526 "parser.y" /* yacc.c:1646  */
    {
		  	Tac t = tacCreate(TAC_UNDEF);
			(yyval.tac) = t;
		  }
#line 2025 "parser.c" /* yacc.c:1646  */
    break;

  case 68:
#line 531 "parser.y" /* yacc.c:1646  */
    {	
			(yyval.tac) = (yyvsp[0].tac);
		  }
#line 2033 "parser.c" /* yacc.c:1646  */
    break;

  case 69:
#line 537 "parser.y" /* yacc.c:1646  */
    {
		  	Tac t = tacCreate(TAC_UNDEF);
			(yyval.tac) = t;
		  }
#line 2042 "parser.c" /* yacc.c:1646  */
    break;

  case 70:
#line 542 "parser.y" /* yacc.c:1646  */
    {	
		  	(yyval.tac) = (yyvsp[0].tac);
		  }
#line 2050 "parser.c" /* yacc.c:1646  */
    break;

  case 71:
#line 548 "parser.y" /* yacc.c:1646  */
    { (yyval.symb) = (yyvsp[0].symb); }
#line 2056 "parser.c" /* yacc.c:1646  */
    break;

  case 72:
#line 552 "parser.y" /* yacc.c:1646  */
    { (yyval.symb) = NULL; }
#line 2062 "parser.c" /* yacc.c:1646  */
    break;

  case 73:
#line 554 "parser.y" /* yacc.c:1646  */
    { }
#line 2068 "parser.c" /* yacc.c:1646  */
    break;

  case 74:
#line 559 "parser.y" /* yacc.c:1646  */
    {
			Tac t;

			t = tacCreate(TAC_STRING);
			t->t1.sym = (yyvsp[0].symb);
			(yyval.tac) = t;
		  }
#line 2080 "parser.c" /* yacc.c:1646  */
    break;

  case 75:
#line 569 "parser.y" /* yacc.c:1646  */
    {
			List l;
			Tac t;

			/* check if it is in the list
			 * already */
			l = findMacroDefinition((yyvsp[0].symb));
			if (l == NULL)
			  {
			    t = tacCreate(TAC_STRING);
			    t->t1.sym = (yyvsp[0].symb);
			  }
			else
			  {
			    Tac macrotac;

			    macrotac = (Tac) l->data;
			    t = tacCopy(macrotac->t2.tac);
			  }
			(yyval.tac) = t;
		  }
#line 2106 "parser.c" /* yacc.c:1646  */
    break;

  case 76:
#line 593 "parser.y" /* yacc.c:1646  */
    {
		  	Tac t = tacCreate(TAC_STRING);
			t->t1.sym = (yyvsp[-3].symb);
			(yyval.tac) = tacJoin(TAC_FCALL,tacTuple((yyvsp[-1].tac)),t,NULL);
		  }
#line 2116 "parser.c" /* yacc.c:1646  */
    break;

  case 77:
#line 599 "parser.y" /* yacc.c:1646  */
    {
		  	(yyval.tac) = tacJoin(TAC_ENCRYPT,tacTuple((yyvsp[-2].tac)),(yyvsp[0].tac),NULL);
		  }
#line 2124 "parser.c" /* yacc.c:1646  */
    break;

  case 78:
#line 603 "parser.y" /* yacc.c:1646  */
    { 
		  	(yyval.tac) = tacTuple((yyvsp[-1].tac));
		  }
#line 2132 "parser.c" /* yacc.c:1646  */
    break;

  case 79:
#line 607 "parser.y" /* yacc.c:1646  */
    { }
#line 2138 "parser.c" /* yacc.c:1646  */
    break;

  case 80:
#line 611 "parser.y" /* yacc.c:1646  */
    { }
#line 2144 "parser.c" /* yacc.c:1646  */
    break;

  case 81:
#line 613 "parser.y" /* yacc.c:1646  */
    {	(yyval.tac) = tacCat((yyvsp[-2].tac),(yyvsp[0].tac)); }
#line 2150 "parser.c" /* yacc.c:1646  */
    break;

  case 82:
#line 617 "parser.y" /* yacc.c:1646  */
    { }
#line 2156 "parser.c" /* yacc.c:1646  */
    break;

  case 83:
#line 619 "parser.y" /* yacc.c:1646  */
    {	(yyval.tac) = tacCat((yyvsp[-2].tac),(yyvsp[0].tac)); }
#line 2162 "parser.c" /* yacc.c:1646  */
    break;

  case 84:
#line 623 "parser.y" /* yacc.c:1646  */
    { }
#line 2168 "parser.c" /* yacc.c:1646  */
    break;


#line 2172 "parser.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
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

  /* Else will try to reuse lookahead token after shifting the error
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

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
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

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


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

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
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
  return yyresult;
}
#line 628 "parser.y" /* yacc.c:1906  */


//! error handler routing
int yyerror(char *s)
{
	extern int yylineno;	//!< defined and maintained in lex.c
	extern char *yytext;	//!< defined and maintained in lex.c
  
	error ("[%i] %s at symbol '%s'.\n", yylineno, s, yytext);
	return 0;
}


