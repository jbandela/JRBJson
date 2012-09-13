//  Copyright John R. Bandela 2012
//
// Distributed under the Boost Software License, Version 1.0.
//    (See  http://www.boost.org/LICENSE_1_0.txt)

#include <string>
#include <iostream>
#include "json.h"

int main(){
	using namespace jrb_json;
	value d1;
	value d2 = 5;
	value d3 = 5.5;
	value d4 = array(
		1,2,"3",
		object("hello",1)
		("world",array(1,2,3,"5"))
	);
	value d5 = object("hello",object("test",array(1,2,object("another","test"))));

	std::string s1 = d1.get<std::string>();
	std::string s2 = d2.get<std::string>();
	int i3 = d3.get<int>();
	int i4 = d4[2].get<int>();
	std::string sd5 = json_string(d5);
	value vsd5 = parse_json(sd5);
	std::string svsd5 = json_string(d5);
	assert(svsd5==sd5);
	std::string in = "[1,2,\"3\",{\"hello\":1,\"world\":[1,2,3,\"5\"]}]";
	value d6 = parse_json(in);
	std::string sout = json_string(d6); 
	assert(in==sout);
	std::cout << pretty_json_string(d5) <<"\n\n";

	std::string pass1 = "[\r\n    \"JSON Test Pattern pass1\",\r\n    {\"object with 1 member\":[\"array with 1 element\"]},\r\n    {},\r\n    [],\r\n    -42,\r\n    true,\r\n    false,\r\n    null,\r\n    {\r\n        \"integer\": 1234567890,\r\n        \"real\": -9876.543210,\r\n        \"e\": 0.123456789e-12,\r\n        \"E\": 1.234567890E+34,\r\n        \"\":  23456789012E66,\r\n        \"zero\": 0,\r\n        \"one\": 1,\r\n        \"space\": \" \",\r\n        \"quote\": \"\\\"\",\r\n        \"backslash\": \"\\\\\",\r\n        \"controls\": \"\\b\\f\\n\\r\\t\",\r\n        \"slash\": \"/ & \\/\",\r\n        \"alpha\": \"abcdefghijklmnopqrstuvwyz\",\r\n        \"ALPHA\": \"ABCDEFGHIJKLMNOPQRSTUVWYZ\",\r\n        \"digit\": \"0123456789\",\r\n        \"0123456789\": \"digit\",\r\n        \"special\": \"`1~!@#$%^&*()_+-={':[,]}|;.</>?\",\r\n        \"hex\": \"\\u0123\\u4567\\u89AB\\uCDEF\\uabcd\\uef4A\",\r\n        \"true\": true,\r\n        \"false\": false,\r\n        \"null\": null,\r\n        \"array\":[  ],\r\n        \"object\":{  },\r\n        \"address\": \"50 St. James Street\",\r\n        \"url\": \"http://www.JSON.org/\",\r\n        \"comment\": \"// /* <!-- --\",\r\n        \"# -- --> */\": \" \",\r\n        \" s p a c e d \" :[1,2 , 3\r\n\r\n,\r\n\r\n4 , 5        ,          6           ,7        ],\"compact\":[1,2,3,4,5,6,7],\r\n        \"jsontext\": \"{\\\"object with 1 member\\\":[\\\"array with 1 element\\\"]}\",\r\n        \"quotes\": \"&#34; \\u0022 %22 0x22 034 &#x22;\",\r\n        \"\\/\\\\\\\"\\uCAFE\\uBABE\\uAB98\\uFCDE\\ubcda\\uef4A\\b\\f\\n\\r\\t`1~!@#$%^&*()_+-=[]{}|;:',./<>?\"\r\n: \"A key can be any string\"\r\n    },\r\n    0.5 ,98.6\r\n,\r\n99.44\r\n,\r\n\r\n1066,\r\n1e1,\r\n0.1e1,\r\n1e-1,\r\n1e00,2e+00,2e-00\r\n,\"rosebud\"]";
	value d7 = parse_json(pass1);
	std::string sd7 = json_string(d7);
	value vsd7 = parse_json(sd7);
	std::string svsd7 = json_string(d7);
	assert(sd7 == svsd7);

	std::cout << pretty_json_string(d7) << "\n\n";
	std::string pass2 = "[[[[[[[[[[[[[[[[[[[\"Not too deep\"]]]]]]]]]]]]]]]]]]]";
		value d8 = parse_json(pass2);
	std::string sd8 = json_string(d8);
	value vsd8 = parse_json(sd8);
	std::string svsd8 = json_string(d8);
	assert(sd8 == svsd8);

	std::cout << pretty_json_string(d8) << "\n\n";



	std::string pass3 = "{\r\n    \"JSON Test Pattern pass3\": {\r\n        \"The outermost value\": \"must be an object or array.\",\r\n        \"In this test\": \"It is an object.\"\r\n    }\r\n}";
	value d9 = parse_json(pass3);
	std::string sd9 = json_string(d9);
	value vsd9 = parse_json(sd9);
	std::string svsd9 = json_string(d9);
	assert(sd9 == svsd9);

	std::cout << pretty_json_string(d9) << "\n\n";
	

	value a1 = array(1,2,3,4);
	value o1 = object("a",1)("b",2)("c",3);
	for(auto i = a1.array_begin(); i != a1.array_end(); i++){
		std::cout << i->get<int>() << " ";
	}
	std::cout << "\n\n";
	
	for(auto i = o1.object_begin(); i != o1.object_end(); i++){
		auto v = i->second;
		std::cout << i->first << " " << v.get<int>() << " ";
	}
	std::cout << "\n\n";

	int k;
	std::cin >> k;

}