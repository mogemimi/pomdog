//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_pp.hpp
 * @brief		iris unit test プリプロセッサ 定義 ファイル
 *
 * @author		t.sirayanagi
 * @version		1.0
 *
 * @par			copyright
 * Copyright (C) 2011-2013, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_iutest_pp_HPP_E8F72184_9C72_4e74_B581_A1F51F291F3A_
#define INCG_IRIS_iutest_pp_HPP_E8F72184_9C72_4e74_B581_A1F51F291F3A_

//======================================================================
// define

/**
 * @brief	コピー禁止定義
*/
#define IUTEST_PP_DISALLOW_COPY_AND_ASSIGN(TypeName)	\
	TypeName( const TypeName& );						\
	TypeName& operator = ( const TypeName& )

/**
 * @brief	代入禁止定義
*/
#define IUTEST_PP_DISALLOW_ASSIGN(TypeName)		\
	TypeName& operator = ( const TypeName& )

/**
 * @brief	コピー/ムーブ禁止定義
*/
#if defined(IUTEST_HAS_RVALUE_REFS) && IUTEST_HAS_RVALUE_REFS
#  define IUTEST_PP_DISALLOW_MOVE_AND_COPY_AND_ASSIGN(TypeName)	\
	IUTEST_PP_DISALLOW_COPY_AND_ASSIGN(TypeName);				\
	TypeName( TypeName&& rhs );									\
	TypeName& operator = ( TypeName&& )
#else
#  define IUTEST_PP_DISALLOW_MOVE_AND_COPY_AND_ASSIGN(TypeName)	\
	IUTEST_PP_DISALLOW_COPY_AND_ASSIGN(TypeName)
#endif

#ifndef IUTEST_PP_TOSTRING
#  define IUTEST_PP_TOSTRING(z_)	IUTEST_PP_TOSTRING_(z_)
#endif
#define IUTEST_PP_TOSTRING_(z_)		#z_

#ifndef IUTEST_PP_CAT
#  define IUTEST_PP_CAT(a_, b_)		IUTEST_PP_CAT_(a_, b_)
#endif
#define IUTEST_PP_CAT_(a_, b_)		a_##b_

#define IUTEST_PP_EXPAND(x)			x

#define IUTEST_PP_COUNTOF(x)		(sizeof(x)/sizeof(x[0]))

#ifdef IUTEST_HAS_COUNTER_MACRO
#  define IUTEST_PP_COUNTER			__COUNTER__
#else
#  define IUTEST_PP_COUNTER			__LINE__
#endif

#define IUTEST_UNUSED_VAR(x)		(void)(x)

// DEC
#define IUTEST_PP_DEC(n)	IUTEST_PP_DEC_I(n)
#define IUTEST_PP_DEC_I(n)	IUTEST_PP_DEC_##n
#define IUTEST_PP_DEC_0		nil
#define IUTEST_PP_DEC_1		0
#define IUTEST_PP_DEC_2		1
#define IUTEST_PP_DEC_3		2
#define IUTEST_PP_DEC_4		3
#define IUTEST_PP_DEC_5		4
#define IUTEST_PP_DEC_6		5
#define IUTEST_PP_DEC_7		6
#define IUTEST_PP_DEC_8		7
#define IUTEST_PP_DEC_9		8
#define IUTEST_PP_DEC_10	9
#define IUTEST_PP_DEC_11	10
#define IUTEST_PP_DEC_12	11
#define IUTEST_PP_DEC_13	12
#define IUTEST_PP_DEC_14	13
#define IUTEST_PP_DEC_15	14
#define IUTEST_PP_DEC_16	15
#define IUTEST_PP_DEC_17	16
#define IUTEST_PP_DEC_18	17
#define IUTEST_PP_DEC_19	18
#define IUTEST_PP_DEC_20	19
#define IUTEST_PP_DEC_21	20
#define IUTEST_PP_DEC_22	21
#define IUTEST_PP_DEC_23	22
#define IUTEST_PP_DEC_24	23
#define IUTEST_PP_DEC_25	24
#define IUTEST_PP_DEC_26	25
#define IUTEST_PP_DEC_27	26
#define IUTEST_PP_DEC_28	27
#define IUTEST_PP_DEC_29	28
#define IUTEST_PP_DEC_30	29
#define IUTEST_PP_DEC_31	30
#define IUTEST_PP_DEC_32	31
#define IUTEST_PP_DEC_33	32
#define IUTEST_PP_DEC_34	33
#define IUTEST_PP_DEC_35	34
#define IUTEST_PP_DEC_36	35
#define IUTEST_PP_DEC_37	36
#define IUTEST_PP_DEC_38	37
#define IUTEST_PP_DEC_39	38
#define IUTEST_PP_DEC_40	39
#define IUTEST_PP_DEC_41	40
#define IUTEST_PP_DEC_42	41
#define IUTEST_PP_DEC_43	42
#define IUTEST_PP_DEC_44	43
#define IUTEST_PP_DEC_45	44
#define IUTEST_PP_DEC_46	45
#define IUTEST_PP_DEC_47	46
#define IUTEST_PP_DEC_48	47
#define IUTEST_PP_DEC_49	48
#define IUTEST_PP_DEC_50	49
#define IUTEST_PP_DEC_51	50
#define IUTEST_PP_DEC_52	51
#define IUTEST_PP_DEC_53	52
#define IUTEST_PP_DEC_54	53
#define IUTEST_PP_DEC_55	54
#define IUTEST_PP_DEC_56	55
#define IUTEST_PP_DEC_57	56
#define IUTEST_PP_DEC_58	57
#define IUTEST_PP_DEC_59	58
#define IUTEST_PP_DEC_60	59
#define IUTEST_PP_DEC_61	60
#define IUTEST_PP_DEC_62	61
#define IUTEST_PP_DEC_63	62
#define IUTEST_PP_DEC_64	63
#define IUTEST_PP_DEC_65	64
#define IUTEST_PP_DEC_66	65
#define IUTEST_PP_DEC_67	66
#define IUTEST_PP_DEC_68	67
#define IUTEST_PP_DEC_69	68
#define IUTEST_PP_DEC_70	69
#define IUTEST_PP_DEC_71	70
#define IUTEST_PP_DEC_72	71
#define IUTEST_PP_DEC_73	72
#define IUTEST_PP_DEC_74	73
#define IUTEST_PP_DEC_75	74
#define IUTEST_PP_DEC_76	75
#define IUTEST_PP_DEC_77	76
#define IUTEST_PP_DEC_78	77
#define IUTEST_PP_DEC_79	78
#define IUTEST_PP_DEC_80	79
#define IUTEST_PP_DEC_81	80
#define IUTEST_PP_DEC_82	81
#define IUTEST_PP_DEC_83	82
#define IUTEST_PP_DEC_84	83
#define IUTEST_PP_DEC_85	84
#define IUTEST_PP_DEC_86	85
#define IUTEST_PP_DEC_87	86
#define IUTEST_PP_DEC_88	87
#define IUTEST_PP_DEC_89	88
#define IUTEST_PP_DEC_90	89
#define IUTEST_PP_DEC_91	90
#define IUTEST_PP_DEC_92	91
#define IUTEST_PP_DEC_93	92
#define IUTEST_PP_DEC_94	93
#define IUTEST_PP_DEC_95	94
#define IUTEST_PP_DEC_96	95
#define IUTEST_PP_DEC_97	96
#define IUTEST_PP_DEC_98	97
#define IUTEST_PP_DEC_99	98
#define IUTEST_PP_DEC_100	99

// INC
#define IUTEST_PP_INC(n)	IUTEST_PP_INC_I(n)
#define IUTEST_PP_INC_I(n)	IUTEST_PP_INC_##n
#define IUTEST_PP_INC_0		1
#define IUTEST_PP_INC_1		2
#define IUTEST_PP_INC_2		3
#define IUTEST_PP_INC_3		4
#define IUTEST_PP_INC_4		5
#define IUTEST_PP_INC_5		6
#define IUTEST_PP_INC_6		7
#define IUTEST_PP_INC_7		8
#define IUTEST_PP_INC_8		9
#define IUTEST_PP_INC_9		10
#define IUTEST_PP_INC_10	11
#define IUTEST_PP_INC_11	12
#define IUTEST_PP_INC_12	13
#define IUTEST_PP_INC_13	14
#define IUTEST_PP_INC_14	15
#define IUTEST_PP_INC_15	16
#define IUTEST_PP_INC_16	17
#define IUTEST_PP_INC_17	18
#define IUTEST_PP_INC_18	19
#define IUTEST_PP_INC_19	20
#define IUTEST_PP_INC_20	21
#define IUTEST_PP_INC_21	22
#define IUTEST_PP_INC_22	23
#define IUTEST_PP_INC_23	24
#define IUTEST_PP_INC_24	25
#define IUTEST_PP_INC_25	26
#define IUTEST_PP_INC_26	27
#define IUTEST_PP_INC_27	28
#define IUTEST_PP_INC_28	29
#define IUTEST_PP_INC_29	30
#define IUTEST_PP_INC_30	31
#define IUTEST_PP_INC_31	32
#define IUTEST_PP_INC_32	33
#define IUTEST_PP_INC_33	34
#define IUTEST_PP_INC_34	35
#define IUTEST_PP_INC_35	36
#define IUTEST_PP_INC_36	37
#define IUTEST_PP_INC_37	38
#define IUTEST_PP_INC_38	39
#define IUTEST_PP_INC_39	40
#define IUTEST_PP_INC_40	41
#define IUTEST_PP_INC_41	42
#define IUTEST_PP_INC_42	43
#define IUTEST_PP_INC_43	44
#define IUTEST_PP_INC_44	45
#define IUTEST_PP_INC_45	46
#define IUTEST_PP_INC_46	47
#define IUTEST_PP_INC_47	48
#define IUTEST_PP_INC_48	49
#define IUTEST_PP_INC_49	50
#define IUTEST_PP_INC_50	51
#define IUTEST_PP_INC_51	52
#define IUTEST_PP_INC_52	53
#define IUTEST_PP_INC_53	54
#define IUTEST_PP_INC_54	55
#define IUTEST_PP_INC_55	56
#define IUTEST_PP_INC_56	57
#define IUTEST_PP_INC_57	58
#define IUTEST_PP_INC_58	59
#define IUTEST_PP_INC_59	60
#define IUTEST_PP_INC_60	61
#define IUTEST_PP_INC_61	62
#define IUTEST_PP_INC_62	63
#define IUTEST_PP_INC_63	64
#define IUTEST_PP_INC_64	65
#define IUTEST_PP_INC_65	66
#define IUTEST_PP_INC_66	67
#define IUTEST_PP_INC_67	68
#define IUTEST_PP_INC_68	69
#define IUTEST_PP_INC_69	70
#define IUTEST_PP_INC_70	71
#define IUTEST_PP_INC_71	72
#define IUTEST_PP_INC_72	73
#define IUTEST_PP_INC_73	74
#define IUTEST_PP_INC_74	75
#define IUTEST_PP_INC_75	76
#define IUTEST_PP_INC_76	77
#define IUTEST_PP_INC_77	78
#define IUTEST_PP_INC_78	79
#define IUTEST_PP_INC_79	80
#define IUTEST_PP_INC_80	81
#define IUTEST_PP_INC_81	82
#define IUTEST_PP_INC_82	83
#define IUTEST_PP_INC_83	84
#define IUTEST_PP_INC_84	85
#define IUTEST_PP_INC_85	86
#define IUTEST_PP_INC_86	87
#define IUTEST_PP_INC_87	88
#define IUTEST_PP_INC_88	89
#define IUTEST_PP_INC_89	90
#define IUTEST_PP_INC_90	91
#define IUTEST_PP_INC_91	92
#define IUTEST_PP_INC_92	93
#define IUTEST_PP_INC_93	94
#define IUTEST_PP_INC_94	95
#define IUTEST_PP_INC_95	96
#define IUTEST_PP_INC_96	97
#define IUTEST_PP_INC_97	98
#define IUTEST_PP_INC_98	99
#define IUTEST_PP_INC_99	100
#define IUTEST_PP_INC_100	nil

// REPEAT
#define IUTEST_PP_REPEAT_PARAMS_MACRO_(i, param)	param
#define IUTEST_PP_REPEAT_PARAMS_MACRO_IN_BINARY_(i, param1, param2)	param2(i, param1)

#define IUTEST_PP_REPEAT_PARAMS(n, param)			IUTEST_PP_REPEAT(n, IUTEST_PP_REPEAT_PARAMS_MACRO_, param)
#define IUTEST_PP_REPEAT(n, m, param)				IUTEST_PP_REPEAT_BINARY(n, IUTEST_PP_REPEAT_PARAMS_MACRO_IN_BINARY_, param, m)

#define IUTEST_PP_REPEAT_BINARY_PARAMS_MACRO_(i, param1, param2)	param1 param2

#define IUTEST_PP_REPEAT_BINARY_PARAMS(n, param1, param2)	IUTEST_PP_REPEAT_BINARY(n, IUTEST_PP_REPEAT_BINARY_PARAMS_MACRO_, param1, param2)
#define IUTEST_PP_REPEAT_BINARY(n, m, param1, param2)		IUTEST_PP_REPEAT_BINARY_PARAMS_I(n, m, param1, param2)

#define IUTEST_PP_REPEAT_BINARY_PARAMS_I(n, m, p1, p2)	IUTEST_PP_CAT(IUTEST_PP_REPEAT_I_, IUTEST_PP_DEC(n))(0, m, p1, p2)

#define IUTEST_PP_REPEAT_I_nil(i, m, p1, p2)
#define IUTEST_PP_REPEAT_I_0( i, m, p1, p2)		m(i, p1, p2)
#define IUTEST_PP_REPEAT_I_1( i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_0 (IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_2( i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_1 (IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_3( i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_2 (IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_4( i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_3 (IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_5( i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_4 (IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_6( i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_5 (IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_7( i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_6 (IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_8( i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_7 (IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_9( i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_8 (IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_10(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_9 (IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_11(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_10(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_12(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_11(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_13(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_12(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_14(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_13(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_15(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_14(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_16(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_15(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_17(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_16(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_18(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_17(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_19(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_18(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_20(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_19(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_21(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_20(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_22(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_21(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_23(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_22(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_24(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_23(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_25(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_24(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_26(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_25(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_27(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_26(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_28(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_27(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_29(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_28(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_30(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_29(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_31(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_30(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_32(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_31(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_33(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_32(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_34(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_33(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_35(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_34(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_36(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_35(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_37(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_36(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_38(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_37(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_39(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_38(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_40(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_39(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_41(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_40(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_42(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_41(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_43(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_42(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_44(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_43(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_45(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_44(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_46(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_45(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_47(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_46(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_48(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_47(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_49(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_48(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_REPEAT_I_50(i, m, p1, p2)		m(i, p1, p2) IUTEST_PP_REPEAT_I_49(IUTEST_PP_INC(i), m, p1, p2)

// ENUM
#define IUTEST_PP_ENUM_PARAMS_MACRO_(i, param)	IUTEST_PP_CAT(param, i)
#define IUTEST_PP_ENUM_PARAMS_MACRO_IN_BINARY_(i, param1, param2)	param2(i, param1)

#define IUTEST_PP_ENUM_PARAMS(n, param)			IUTEST_PP_ENUM(n, IUTEST_PP_ENUM_PARAMS_MACRO_, param)
#define IUTEST_PP_ENUM(n, m, param)				IUTEST_PP_ENUM_BINARY(n, IUTEST_PP_ENUM_PARAMS_MACRO_IN_BINARY_, param, m)

#define IUTEST_PP_ENUM_SHIFTED_PARAMS(n, param)	IUTEST_PP_ENUM_SHIFTED(n, IUTEST_PP_ENUM_PARAMS_MACRO_, param)
#define IUTEST_PP_ENUM_SHIFTED(n, m, param)		IUTEST_PP_ENUM_SHIFTED_BINARY(n, IUTEST_PP_ENUM_PARAMS_MACRO_IN_BINARY_, param, m)

#define IUTEST_PP_ENUM_BINARY_PARAMS_MACRO_(i, param1, param2)	IUTEST_PP_CAT(param1, i) IUTEST_PP_CAT(param2, i)

#define IUTEST_PP_ENUM_BINARY_PARAMS(n, param1, param2)			IUTEST_PP_ENUM_BINARY(n, IUTEST_PP_ENUM_BINARY_PARAMS_MACRO_, param1, param2)
#define IUTEST_PP_ENUM_BINARY(n, m, param1, param2)				IUTEST_PP_ENUM_BINARY_PARAMS_I(0, n, m, param1, param2)

#define IUTEST_PP_ENUM_SHIFTED_BINARY_PARAMS(n, param1, param2)	IUTEST_PP_ENUM_BINARY(n, IUTEST_PP_ENUM_BINARY_PARAMS_MACRO_, param1, param2)
#define IUTEST_PP_ENUM_SHIFTED_BINARY(n, m, param1, param2)		IUTEST_PP_ENUM_BINARY_PARAMS_I(1, n, m, param1, param2)

#define IUTEST_PP_ENUM_BINARY_PARAMS_I(begin, n, m, p1, p2)	IUTEST_PP_CAT(IUTEST_PP_ENUM_I_, IUTEST_PP_DEC(n))(begin, m, p1, p2)

#define IUTEST_PP_ENUM_I_nil(i, m, p1, p2)	
#define IUTEST_PP_ENUM_I_0( i, m, p1, p2)	m(i, p1, p2)
#define IUTEST_PP_ENUM_I_1( i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_0 (IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_2( i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_1 (IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_3( i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_2 (IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_4( i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_3 (IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_5( i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_4 (IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_6( i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_5 (IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_7( i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_6 (IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_8( i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_7 (IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_9( i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_8 (IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_10(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_9 (IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_11(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_10(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_12(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_11(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_13(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_12(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_14(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_13(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_15(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_14(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_16(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_15(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_17(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_16(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_18(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_17(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_19(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_18(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_20(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_19(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_21(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_20(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_22(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_21(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_23(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_22(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_24(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_23(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_25(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_24(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_26(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_25(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_27(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_26(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_28(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_27(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_29(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_28(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_30(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_29(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_31(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_30(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_32(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_31(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_33(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_32(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_34(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_33(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_35(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_34(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_36(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_35(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_37(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_36(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_38(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_37(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_39(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_38(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_40(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_39(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_41(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_40(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_42(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_41(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_43(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_42(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_44(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_43(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_45(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_44(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_46(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_45(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_47(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_46(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_48(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_47(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_49(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_48(IUTEST_PP_INC(i), m, p1, p2)
#define IUTEST_PP_ENUM_I_50(i, m, p1, p2)	m(i, p1, p2), IUTEST_PP_ENUM_I_49(IUTEST_PP_INC(i), m, p1, p2)

// BOOL
#define IUTEST_PP_BOOL(n)	IUTEST_PP_BOOL_I(n)
#define IUTEST_PP_BOOL_I(n)	IUTEST_PP_BOOL_##n
#define IUTEST_PP_BOOL_0	0
#define IUTEST_PP_BOOL_1	1
#define IUTEST_PP_BOOL_2	1
#define IUTEST_PP_BOOL_3	1
#define IUTEST_PP_BOOL_4	1
#define IUTEST_PP_BOOL_5	1
#define IUTEST_PP_BOOL_6	1
#define IUTEST_PP_BOOL_7	1
#define IUTEST_PP_BOOL_8	1
#define IUTEST_PP_BOOL_9	1
#define IUTEST_PP_BOOL_10	1
#define IUTEST_PP_BOOL_11	1
#define IUTEST_PP_BOOL_12	1
#define IUTEST_PP_BOOL_13	1
#define IUTEST_PP_BOOL_14	1
#define IUTEST_PP_BOOL_15	1
#define IUTEST_PP_BOOL_16	1
#define IUTEST_PP_BOOL_17	1
#define IUTEST_PP_BOOL_18	1
#define IUTEST_PP_BOOL_19	1
#define IUTEST_PP_BOOL_20	1
#define IUTEST_PP_BOOL_21	1
#define IUTEST_PP_BOOL_22	1
#define IUTEST_PP_BOOL_23	1
#define IUTEST_PP_BOOL_24	1
#define IUTEST_PP_BOOL_25	1
#define IUTEST_PP_BOOL_26	1
#define IUTEST_PP_BOOL_27	1
#define IUTEST_PP_BOOL_28	1
#define IUTEST_PP_BOOL_29	1
#define IUTEST_PP_BOOL_30	1
#define IUTEST_PP_BOOL_31	1
#define IUTEST_PP_BOOL_32	1
#define IUTEST_PP_BOOL_33	1
#define IUTEST_PP_BOOL_34	1
#define IUTEST_PP_BOOL_35	1
#define IUTEST_PP_BOOL_36	1
#define IUTEST_PP_BOOL_37	1
#define IUTEST_PP_BOOL_38	1
#define IUTEST_PP_BOOL_39	1
#define IUTEST_PP_BOOL_40	1
#define IUTEST_PP_BOOL_41	1
#define IUTEST_PP_BOOL_42	1
#define IUTEST_PP_BOOL_43	1
#define IUTEST_PP_BOOL_44	1
#define IUTEST_PP_BOOL_45	1
#define IUTEST_PP_BOOL_46	1
#define IUTEST_PP_BOOL_47	1
#define IUTEST_PP_BOOL_48	1
#define IUTEST_PP_BOOL_49	1
#define IUTEST_PP_BOOL_50	1
#define IUTEST_PP_BOOL_51	1
#define IUTEST_PP_BOOL_52	1
#define IUTEST_PP_BOOL_53	1
#define IUTEST_PP_BOOL_54	1
#define IUTEST_PP_BOOL_55	1
#define IUTEST_PP_BOOL_56	1
#define IUTEST_PP_BOOL_57	1
#define IUTEST_PP_BOOL_58	1
#define IUTEST_PP_BOOL_59	1
#define IUTEST_PP_BOOL_60	1
#define IUTEST_PP_BOOL_61	1
#define IUTEST_PP_BOOL_62	1
#define IUTEST_PP_BOOL_63	1
#define IUTEST_PP_BOOL_64	1
#define IUTEST_PP_BOOL_65	1
#define IUTEST_PP_BOOL_66	1
#define IUTEST_PP_BOOL_67	1
#define IUTEST_PP_BOOL_68	1
#define IUTEST_PP_BOOL_69	1
#define IUTEST_PP_BOOL_70	1
#define IUTEST_PP_BOOL_71	1
#define IUTEST_PP_BOOL_72	1
#define IUTEST_PP_BOOL_73	1
#define IUTEST_PP_BOOL_74	1
#define IUTEST_PP_BOOL_75	1
#define IUTEST_PP_BOOL_76	1
#define IUTEST_PP_BOOL_77	1
#define IUTEST_PP_BOOL_78	1
#define IUTEST_PP_BOOL_79	1
#define IUTEST_PP_BOOL_80	1
#define IUTEST_PP_BOOL_81	1
#define IUTEST_PP_BOOL_82	1
#define IUTEST_PP_BOOL_83	1
#define IUTEST_PP_BOOL_84	1
#define IUTEST_PP_BOOL_85	1
#define IUTEST_PP_BOOL_86	1
#define IUTEST_PP_BOOL_87	1
#define IUTEST_PP_BOOL_88	1
#define IUTEST_PP_BOOL_89	1
#define IUTEST_PP_BOOL_90	1
#define IUTEST_PP_BOOL_91	1
#define IUTEST_PP_BOOL_92	1
#define IUTEST_PP_BOOL_93	1
#define IUTEST_PP_BOOL_94	1
#define IUTEST_PP_BOOL_95	1
#define IUTEST_PP_BOOL_96	1
#define IUTEST_PP_BOOL_97	1
#define IUTEST_PP_BOOL_98	1
#define IUTEST_PP_BOOL_99	1
#define IUTEST_PP_BOOL_100	1
#define IUTEST_PP_BOOL_101	1
#define IUTEST_PP_BOOL_102	1
#define IUTEST_PP_BOOL_103	1
#define IUTEST_PP_BOOL_104	1
#define IUTEST_PP_BOOL_105	1
#define IUTEST_PP_BOOL_106	1
#define IUTEST_PP_BOOL_107	1
#define IUTEST_PP_BOOL_108	1
#define IUTEST_PP_BOOL_109	1
#define IUTEST_PP_BOOL_110	1
#define IUTEST_PP_BOOL_111	1
#define IUTEST_PP_BOOL_112	1
#define IUTEST_PP_BOOL_113	1
#define IUTEST_PP_BOOL_114	1
#define IUTEST_PP_BOOL_115	1
#define IUTEST_PP_BOOL_116	1
#define IUTEST_PP_BOOL_117	1
#define IUTEST_PP_BOOL_118	1
#define IUTEST_PP_BOOL_119	1
#define IUTEST_PP_BOOL_120	1
#define IUTEST_PP_BOOL_121	1
#define IUTEST_PP_BOOL_122	1
#define IUTEST_PP_BOOL_123	1
#define IUTEST_PP_BOOL_124	1
#define IUTEST_PP_BOOL_125	1
#define IUTEST_PP_BOOL_126	1
#define IUTEST_PP_BOOL_127	1
#define IUTEST_PP_BOOL_128	1
#define IUTEST_PP_BOOL_129	1
#define IUTEST_PP_BOOL_130	1
#define IUTEST_PP_BOOL_131	1
#define IUTEST_PP_BOOL_132	1
#define IUTEST_PP_BOOL_133	1
#define IUTEST_PP_BOOL_134	1
#define IUTEST_PP_BOOL_135	1
#define IUTEST_PP_BOOL_136	1
#define IUTEST_PP_BOOL_137	1
#define IUTEST_PP_BOOL_138	1
#define IUTEST_PP_BOOL_139	1
#define IUTEST_PP_BOOL_140	1
#define IUTEST_PP_BOOL_141	1
#define IUTEST_PP_BOOL_142	1
#define IUTEST_PP_BOOL_143	1
#define IUTEST_PP_BOOL_144	1
#define IUTEST_PP_BOOL_145	1
#define IUTEST_PP_BOOL_146	1
#define IUTEST_PP_BOOL_147	1
#define IUTEST_PP_BOOL_148	1
#define IUTEST_PP_BOOL_149	1
#define IUTEST_PP_BOOL_150	1
#define IUTEST_PP_BOOL_151	1
#define IUTEST_PP_BOOL_152	1
#define IUTEST_PP_BOOL_153	1
#define IUTEST_PP_BOOL_154	1
#define IUTEST_PP_BOOL_155	1
#define IUTEST_PP_BOOL_156	1
#define IUTEST_PP_BOOL_157	1
#define IUTEST_PP_BOOL_158	1
#define IUTEST_PP_BOOL_159	1
#define IUTEST_PP_BOOL_160	1
#define IUTEST_PP_BOOL_161	1
#define IUTEST_PP_BOOL_162	1
#define IUTEST_PP_BOOL_163	1
#define IUTEST_PP_BOOL_164	1
#define IUTEST_PP_BOOL_165	1
#define IUTEST_PP_BOOL_166	1
#define IUTEST_PP_BOOL_167	1
#define IUTEST_PP_BOOL_168	1
#define IUTEST_PP_BOOL_169	1
#define IUTEST_PP_BOOL_170	1
#define IUTEST_PP_BOOL_171	1
#define IUTEST_PP_BOOL_172	1
#define IUTEST_PP_BOOL_173	1
#define IUTEST_PP_BOOL_174	1
#define IUTEST_PP_BOOL_175	1
#define IUTEST_PP_BOOL_176	1
#define IUTEST_PP_BOOL_177	1
#define IUTEST_PP_BOOL_178	1
#define IUTEST_PP_BOOL_179	1
#define IUTEST_PP_BOOL_180	1
#define IUTEST_PP_BOOL_181	1
#define IUTEST_PP_BOOL_182	1
#define IUTEST_PP_BOOL_183	1
#define IUTEST_PP_BOOL_184	1
#define IUTEST_PP_BOOL_185	1
#define IUTEST_PP_BOOL_186	1
#define IUTEST_PP_BOOL_187	1
#define IUTEST_PP_BOOL_188	1
#define IUTEST_PP_BOOL_189	1
#define IUTEST_PP_BOOL_190	1
#define IUTEST_PP_BOOL_191	1
#define IUTEST_PP_BOOL_192	1
#define IUTEST_PP_BOOL_193	1
#define IUTEST_PP_BOOL_194	1
#define IUTEST_PP_BOOL_195	1
#define IUTEST_PP_BOOL_196	1
#define IUTEST_PP_BOOL_197	1
#define IUTEST_PP_BOOL_198	1
#define IUTEST_PP_BOOL_199	1
#define IUTEST_PP_BOOL_200	1
#define IUTEST_PP_BOOL_201	1
#define IUTEST_PP_BOOL_202	1
#define IUTEST_PP_BOOL_203	1
#define IUTEST_PP_BOOL_204	1
#define IUTEST_PP_BOOL_205	1
#define IUTEST_PP_BOOL_206	1
#define IUTEST_PP_BOOL_207	1
#define IUTEST_PP_BOOL_208	1
#define IUTEST_PP_BOOL_209	1
#define IUTEST_PP_BOOL_210	1
#define IUTEST_PP_BOOL_211	1
#define IUTEST_PP_BOOL_212	1
#define IUTEST_PP_BOOL_213	1
#define IUTEST_PP_BOOL_214	1
#define IUTEST_PP_BOOL_215	1
#define IUTEST_PP_BOOL_216	1
#define IUTEST_PP_BOOL_217	1
#define IUTEST_PP_BOOL_218	1
#define IUTEST_PP_BOOL_219	1
#define IUTEST_PP_BOOL_220	1
#define IUTEST_PP_BOOL_221	1
#define IUTEST_PP_BOOL_222	1
#define IUTEST_PP_BOOL_223	1
#define IUTEST_PP_BOOL_224	1
#define IUTEST_PP_BOOL_225	1
#define IUTEST_PP_BOOL_226	1
#define IUTEST_PP_BOOL_227	1
#define IUTEST_PP_BOOL_228	1
#define IUTEST_PP_BOOL_229	1
#define IUTEST_PP_BOOL_230	1
#define IUTEST_PP_BOOL_231	1
#define IUTEST_PP_BOOL_232	1
#define IUTEST_PP_BOOL_233	1
#define IUTEST_PP_BOOL_234	1
#define IUTEST_PP_BOOL_235	1
#define IUTEST_PP_BOOL_236	1
#define IUTEST_PP_BOOL_237	1
#define IUTEST_PP_BOOL_238	1
#define IUTEST_PP_BOOL_239	1
#define IUTEST_PP_BOOL_240	1
#define IUTEST_PP_BOOL_241	1
#define IUTEST_PP_BOOL_242	1
#define IUTEST_PP_BOOL_243	1
#define IUTEST_PP_BOOL_244	1
#define IUTEST_PP_BOOL_245	1
#define IUTEST_PP_BOOL_246	1
#define IUTEST_PP_BOOL_247	1
#define IUTEST_PP_BOOL_248	1
#define IUTEST_PP_BOOL_249	1
#define IUTEST_PP_BOOL_250	1
#define IUTEST_PP_BOOL_251	1
#define IUTEST_PP_BOOL_252	1
#define IUTEST_PP_BOOL_253	1
#define IUTEST_PP_BOOL_254	1
#define IUTEST_PP_BOOL_255	1

// IF
#define IUTEST_PP_IF(cond, t, f)	IUTEST_PP_IF_I(IUTEST_PP_BOOL(cond), t, f)
#define IUTEST_PP_IF_I(cond, t, f)	IUTEST_PP_CAT(IUTEST_PP_IF_, cond)(t, f)
#define IUTEST_PP_IF_0(t, f)		f
#define IUTEST_PP_IF_1(t, f)		t

// IS EMPTY
#define IUTEST_PP_IS_EMPTY(x)		IUTEST_PP_IS_EMPTY_I(x IUTEST_PP_EMPTY_TAG)
#define IUTEST_PP_IS_EMPTY_I(x)		IUTEST_PP_IS_EMPTY_I_( IUTEST_PP_CAT(IUTEST_PP_EMPTY_TAG_, x)() )
#define IUTEST_PP_IS_EMPTY_I_(x)	IUTEST_PP_IS_EMPTY_II((x))
#define IUTEST_PP_IS_EMPTY_II(x)	IUTEST_PP_IS_EMPTY_II_ x
#define IUTEST_PP_IS_EMPTY_II_(a, b)	b

#define IUTEST_PP_EMPTY()		
#define IUTEST_PP_EMPTY_TAG()	, 0
#define IUTEST_PP_EMPTY_TAG_IUTEST_PP_EMPTY_TAG	1, 1 IUTEST_PP_EMPTY

#endif
