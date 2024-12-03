
int num_rule_sections=2;

int rule_sections[2]={	0,	22	};

int num_rules=69;

rule_t rule_table[69] = {
{	0,	-1,	0xFFFF,	0x0003,	95,	-1,	-1,	-1,	-1,	-1,	"T<1>"	},
{	0,	-1,	0xFFFF,	0x0003,	 0,	 1,	-1,	-1,	19,	-1,	"W~<+>'.'d/E<1>/E<1>W~<*>"	},
{	0,	-1,	0xFFFF,	0x0003,	 1,	 1,	-1,	-1,	-1,	-1,	"W~<+>d/E<1>/E<1>"	},
{	0,	-1,	0xFFFF,	0x0003,	 2,	18,	-1,	-1,	-1,	-1,	"W~<+>E<1>d/P<+>/"	},
{	0,	-1,	0xFFFF,	0x0003,	90,	 1,	-1,	-1,	-1,	-1,	"W~<+>d/P<1>/E<1>"	},
{	0,	-1,	0xFFFF,	0x0003,	 3,	 1,	-1,	-1,	-1,	-1,	"W~<*>($1,P<1>)d/S{$1}<+>/W~<*>"	},
{	0,	-1,	0xFFFF,	0x0002,	121,	 0,	-1,	-1,	-1,	-1,	"h/3,T<1>/d/$7/|a/$7/' '//W~<*>"	},
{	0,	-1,	0xFFFF,	0x0002,	122,	 0,	-1,	-1,	-1,	-1,	"W~<*>h/3,T<1>/d/$7/|b/$7/' '//W~<*>"	},
{	0,	-1,	0xFFFF,	0x0003,	123,	 0,	-1,	-1,	-1,	-1,	"W~<*>a/P<1>/' '/P<1>W~<*>"	},
{	0,	-1,	0xFFFF,	0x0003,	 4,	 0,	-1,	-1,	-1,	-1,	"W~<*>a/P<1>/' '/T<1>W~<*>"	},
{	0,	-1,	0xFFFF,	0x0003,	 5,	 0,	-1,	-1,	-1,	-1,	"W~<*>a/T<1>/' '/T<1>W~<*>"	},
{	0,	-1,	0xFFFF,	0x0003,	 6,	 0,	-1,	-1,	-1,	-1,	"W~<*>a/T<1>/' '/P<1>W~<*>"	},
{	0,	-1,	0xFFFF,	0x0004,	 7,	18,	-1,	-1,	-1,	-1,	"a/E<1>/0x0B/W~<*>"	},
{	0,	-1,	0xFFFF,	0x0004,	91,	 0,	-1,	-1,	-1,	-1,	"a/P<1>/' '/W~<*>"	},
{	0,	-1,	0xFFFF,	0x0004,	 8,	 1,	-1,	-1,	-1,	-1,	"a/W~<+>/' '/P<1>W~<*>"	},
{	0,	-1,	0xFFFF,	0x0002,	 9,	 0,	-1,	-1,	-1,	-1,	"a/T<1>/' '/W~<*>"	},
{	0,	-1,	0xFFFF,	0x0002,	10,	 0,	-1,	-1,	-1,	-1,	"a/W~<+>/' '/T<1>W~<*>"	},
{	0,	-1,	0xFFFF,	0x0001,	11,	17,	-1,	-1,	-1,	-1,	"W~<*>r/T<+>/' '/W~<*>"	},
{	1,	1,	0x0000,	0x0000,	12,	 1,	 1,	 1,	 1,	 0,	""	},
{	0,	-1,	0xFFFF,	0x0003,	101,	21,	20,	-1,	-1,	-1,	"W~<+>E~<*>'.'h/1,E<1>/r/$7/' '$9/|FAIL/"	},
{	2,	1,	0x0000,	0x0000,	102,	 1,	 1,	 1,	 1,	 0,	""	},
{	1,	1,	0x0000,	0x0000,	103,	 1,	 1,	 1,	 1,	 0,	""	},
{	0,	-1,	0x7FFF,	0x7FFF,	15,	-1,	-1,	-1,	-1,	-1,	"o/'1'r/'-'/', '//D[8-9]'00'r/'-'/', '/i/D[2-9]D<2>/' '/r/'-'/', '/i/D<4>/' '/"	},
{	0,	-1,	0x7FFF,	0x7FFF,	16,	-1,	-1,	-1,	-1,	-1,	"o/'1'r/' '/', '//D[8-9]'00'r/' '/', '/i/D[2-9]D<2>/' '/r/' '/', '/i/D<4>/' '/"	},
{	0,	-1,	0x7FFF,	0x7FFF,	17,	-1,	-1,	-1,	-1,	-1,	"d/'('/D[8-9]'00'r/')'W<*>/', '/i/D[2-9]D<2>/' '/r/S{'-',' ',''}<1>/', '/i/D<4>/' '/"	},
{	0,	-1,	0x7FFF,	0x7FFF,	18,	-1,	-1,	-1,	-1,	-1,	"o/a/'1'/', '//a/D[8-9]'00'/', '/a/i/D[2-9]D<2>/' '//', '/a/i/D<4>/' '//', '/"	},
{	0,	-1,	0x7FFF,	0x7FFF,	19,	-1,	-1,	-1,	-1,	-1,	"o/'1'r/'-'/', '//i/D<3>/' '/r/'-'/', '/i/D[2-9]D<2>/' '/r/'-'/', '/i/D<4>/' '/"	},
{	0,	-1,	0x7FFF,	0x7FFF,	20,	-1,	-1,	-1,	-1,	-1,	"o/'1'r/' '/', '//i/D<3>/' '/r/' '/', '/i/D[2-9]D<2>/' '/r/' '/', '/i/D<4>/' '/"	},
{	0,	-1,	0x7FFF,	0x7FFF,	21,	-1,	-1,	-1,	-1,	-1,	"d/'('/i/D<3>/' '/r/')'W<*>/', '/i/D[2-9]D<2>/' '/r/S{'-',' ',''}<1>/', '/i/D<4>/' '/"	},
{	0,	-1,	0x7FFF,	0x7FFF,	22,	-1,	-1,	-1,	-1,	-1,	"o/a/'1'/', '//a/i/D[2-9]D<2>/' '//', '/a/i/D[2-9]D<2>/' '//', '/a/i/D<4>/' '//', '/"	},
{	0,	-1,	0x7FFF,	0x7FFF,	23,	-1,	-1,	-1,	-1,	-1,	"i/D[2-9]D<2>/' '/r/S{'-',' ',''}<1>/', '/i/D<4>/' '/"	},
{	0,	-1,	0x7FFF,	0x7FFF,	24,	-1,	-1,	-1,	-1,	-1,	"'A'r/S{'L','K','Z','R'}<1>/'labama'|'laska'|'rizona'|'rkansas'/W<1-4>i/D<5>/' '/o/r/'-'/' dash '/i/D<4>/' '//"	},
{	0,	-1,	0x7FFF,	0x7FFF,	25,	-1,	-1,	-1,	-1,	-1,	"'C'r/S{'A','O','T'}<1>/'alifornia'|'olorado'|'onnecticut'/W<1-4>i/D<5>/' '/o/r/'-'/' dash '/i/D<4>/' '//"	},
{	0,	-1,	0x7FFF,	0x7FFF,	26,	-1,	-1,	-1,	-1,	-1,	"r/S{'DE','FL','GA','HI'}<1>/'Delaware'|'Florida'|'Georgia'|'Hawaii'/W<1-4>i/D<5>/' '/o/r/'-'/' dash '/i/D<4>/' '//"	},
{	0,	-1,	0x7FFF,	0x7FFF,	27,	-1,	-1,	-1,	-1,	-1,	"'I'r/S{'D','L','N','A'}<1>/'daho'|'llinois'|'ndiana'|'owa'/W<1-4>i/D<5>/' '/o/r/'-'/' dash '/i/D<4>/' '//"	},
{	0,	-1,	0x7FFF,	0x7FFF,	28,	-1,	-1,	-1,	-1,	-1,	"r/S{'KY','KS','LA'}<1>/'Kansas'|'Kentucky'|'Louisiana'|/W<1-4>i/D<5>/' '/o/r/'-'/' dash '/i/D<4>/' '//"	},
{	0,	-1,	0x7FFF,	0x7FFF,	29,	-1,	-1,	-1,	-1,	-1,	"'M'r/S{'E','D','A','I'}<1>/'aine'|'aryland'|'assachusetts'|'ichigan'|/W<1-4>i/D<5>/' '/o/r/'-'/' dash '/i/D<4>/' '//"	},
{	0,	-1,	0x7FFF,	0x7FFF,	30,	-1,	-1,	-1,	-1,	-1,	"'M'r/S{'N','S','O','T'}<1>/'innesota'|'ississippi'|'issouri'|'ontana'|/W<1-4>i/D<5>/' '/o/r/'-'/' dash '/i/D<4>/' '//"	},
{	0,	-1,	0x7FFF,	0x7FFF,	31,	-1,	-1,	-1,	-1,	-1,	"'N'r/S{'E','V','H','J'}<1>/'ebraska'|'evada'|'ew Hampshire'|'ew Jersey'|/W<1-4>i/D<5>/' '/o/r/'-'/' dash '/i/D<4>/' '//"	},
{	0,	-1,	0x7FFF,	0x7FFF,	32,	-1,	-1,	-1,	-1,	-1,	"'N'r/S{'M','Y','C','D'}<1>/'ew Mexico'|'ew York'|'orth Carolina'|'orth Dakota'|/W<1-4>i/D<5>/' '/o/r/'-'/' dash '/i/D<4>/' '//"	},
{	0,	-1,	0x7FFF,	0x7FFF,	33,	-1,	-1,	-1,	-1,	-1,	"'O'r/S{'H','K','R'}<1>/'hio'|'klahoma'|'regon'|/W<1-4>i/D<5>/' '/o/r/'-'/' dash '/i/D<4>/' '//"	},
{	0,	-1,	0x7FFF,	0x7FFF,	34,	-1,	-1,	-1,	-1,	-1,	"r/S{'PA','RI','DC'}<1>/'Pennsylvania'|'Rhode Island'|'District of Columbia'/W<1-4>i/D<5>/' '/o/r/'-'/' dash '/i/D<4>/' '//"	},
{	0,	-1,	0x7FFF,	0x7FFF,	35,	-1,	-1,	-1,	-1,	-1,	"r/S{'SC','SD','TN','TX'}<1>/'South Carolina'|'South Dakota'|'Tennessee'|'Texas'/W<1-4>i/D<5>/' '/o/r/'-'/' dash '/i/D<4>/' '//"	},
{	0,	-1,	0x7FFF,	0x7FFF,	36,	-1,	-1,	-1,	-1,	-1,	"r/S{'UT','VT','VA'}<1>/'Utah'|'Vermont'|'Virginia'|/W<1-4>i/D<5>/' '/o/r/'-'/' dash '/i/D<4>/' '//"	},
{	0,	-1,	0x7FFF,	0x7FFF,	37,	-1,	-1,	-1,	-1,	-1,	"'W'r/S{'A','V','I','Y'}<1>/'ashington'|'est Virginia'|'isconsin'|'yoming'/W<1-4>i/D<5>/' '/o/r/'-'/' dash '/i/D<4>/' '//"	},
{	0,	-1,	0x0001,	0xFFFF,	38,	-1,	-1,	-1,	-1,	-1,	"r/'D'`r.`/'Doctor'/r/W<1-3>/' '/c/S{U<1>L<+>,U<*>}<1>/"	},
{	0,	-1,	0x0001,	0xFFFF,	39,	-1,	-1,	-1,	-1,	-1,	"r/'S'`t.`/'Saint'/r/W<1-3>/' '/c/S{U<1>L<+>,U<*>}<1>/"	},
{	0,	-1,	0xFFFF,	0xFFFF,	41,	57,	-1,	-1,	-1,	 0,	"H<3-20>r/'.'/' dot '/i/S{O<1>V<1>O<1>,A<1>H<0-2>}<1>/''|' '/"	},
{	0,	-1,	0x0001,	0xFFFF,	42,	-1,	-1,	-1,	-1,	-1,	"a/D<1-4>/'th'|'st'|'nd'|'rd'|'th'|'th'|'th'|'th'|'th'|'th'/r/W<1-4>/' '/S{`st.`,`ave.`,`dr.`}<1>"	},
{	0,	-1,	0x0001,	0xFFFF,	120,	-1,	-1,	-1,	-1,	-1,	"D<*>h/2,D<1>S{`th`,`st`,`nd`,`rd`}<1>/$7|FAIL/"	},
{	0,	-1,	0x7FFF,	0x7FFF,	43,	-1,	-1,	-1,	-1,	-1,	"d/($1,r/D[1-12]/'Jan'|'Feb'|'Mar'|'Apr'|'May'|'Jun'|'Jul'|'Aug'|'Sep'|'Oct'|'Nov'|'Dec'/)($2,S{'/','-'}<1>)/D[1-31]r/$2/'-'$1'-'/D<2,4>"	},
{	0,	-1,	0x7FFF,	0x7FFF,	44,	-1,	-1,	-1,	-1,	-1,	"d/($1,S{'J'`an`,'F'`eb`,'M'`ar`,'A'`pr`,'M'`ay`,'J'`un`,'J'`ul`,'A'`ug`,'S'`ep`,'O'`ct`,'N'`ov`,'D'`ec`}<1>)o/'.'/W<1-4>/a/D[1-31]/'-'$1/o/r/o/','/W<1-4>/'-'/D<2,4>/"	},
{	0,	-1,	0x7FFF,	0x7FFF,	45,	-1,	-1,	-1,	-1,	-1,	"D<1-4>r/S{' ','-'}<1>/' and '/D[1-99]'/'D[1-100]"	},
{	0,	-1,	0xFFFF,	0xFFFF,	46,	-1,	-1,	-1,	-1,	 0,	"D<1-4>r/'-'/' '/A<+>"	},
{	0,	-1,	0x7FFF,	0x7FFF,	47,	-1,	-1,	-1,	-1,	 0,	"A<+>r/'/'/'-'/A<+>"	},
{	0,	-1,	0xFFFF,	0xFFFF,	48,	-1,	-1,	-1,	-1,	 0,	"A<+>r/'/'/' '/D<+>"	},
{	0,	-1,	0xFFFF,	0xFFFF,	49,	-1,	-1,	-1,	-1,	 0,	"D<+>r/'/'/' '/A<+>"	},
{	0,	-1,	0x7FFF,	0x7FFF,	50,	57,	-1,	67,	-1,	-1,	"a/D<+>/' '/A<+>r/'-'/' dash '/H<+>W~<*>"	},
{	0,	-1,	0x7FFF,	0x7FFF,	51,	57,	-1,	68,	-1,	-1,	"a/A<+>/' '/D<+>r/'-'/' dash '/H<+>W~<*>"	},
{	0,	-1,	0x7FFF,	0x7FFF,	52,	57,	-1,	65,	-1,	-1,	"a/A<+>/' '/D<1-6>o/'.'D<1-4>/D~<0-1>W~<*>"	},
{	0,	-1,	0x7FFF,	0x7FFF,	53,	57,	-1,	66,	-1,	-1,	"a/D<+>o/'.'D<+>//' '/A<+>W~<*>"	},
{	0,	-1,	0x7FFF,	0x7FFF,	54,	57,	-1,	65,	-1,	-1,	"A<+>r/'-'/' dash '/S{A<+>D<+>,D<+>A<+>}<1>W~<*>"	},
{	0,	-1,	0x7FFF,	0x7FFF,	55,	57,	-1,	-1,	-1,	-1,	"D<+>r/'-'/' dash '/A<+>D<+>W~<*>"	},
{	0,	-1,	0x7FFF,	0x7FFF,	56,	57,	-1,	-1,	-1,	-1,	"D<+>o/'.'D<+>/b/a/'-'/' '//' '/D<+>W~<*>"	},
{	1,	1,	0x0000,	0x0000,	57,	 1,	 1,	 1,	 1,	 0,	""	},
{	0,	-1,	0x0001,	0xFFFF,	58,	-1,	-1,	-1,	-1,	-1,	"r/`a`/'eyh'/"	},
{	0,	-1,	0x0001,	0xFFFF,	59,	-1,	-1,	-1,	-1,	-1,	"W~<*>' 'r/`a`/'eyh'/"	},
{	0,	-1,	0x0001,	0xFFFF,	60,	-1,	-1,	-1,	-1,	-1,	"W~<*>' 'o/r/`a`/'eyh'//' dash 'o/r/`a`/'eyh'//"	},
{	0,	-1,	0x0001,	0xFFFF,	61,	-1,	-1,	-1,	-1,	-1,	"o/r/`a`/'eyh'//W~<*>' dash 'o/r/`a`/'eyh'//"	}
};

dict_pointers_t dict_point[3]={
{	    0,	    5,	    6	},
{	    7,	   16,	   10	},
{	   18,	   24,	    7	}};

struct dict_data_type
{
	unsigned char search_graph[4];
	unsigned char out_graph[4];
};

typedef struct dict_data_type dict_data_type;

dict_data_type	dict_data[25]={
{	"!"  ,	"�!�"	},
{	","  ,	"�,�"	},
{	"."  ,	"�.�"	},
{	":"  ,	"�,�"	},
{	";"  ,	"�,�"	},
{	"?"  ,	"�?�"	},
{	""   ,	""   	},
{	"0th",	""   	},
{	"1st",	""   	},
{	"2nd",	""   	},
{	"3rd",	""   	},
{	"4th",	""   	},
{	"5th",	""   	},
{	"6th",	""   	},
{	"7th",	""   	},
{	"8th",	""   	},
{	"9th",	""   	},
{	""   ,	""   	},
{	"("  ,	"("  	},
{	")"  ,	")"  	},
{	"<"  ,	"("  	},
{	">"  ,	")"  	},
{	"]"  ,	")"  	},
{	"{"  ,	"("  	},
{	"}"  ,	")"  	}};

