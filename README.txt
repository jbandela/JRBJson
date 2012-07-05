OVERVIEW

This library is a json library using c++. It is inspired by the facebook folly library. It uses rapidjson to handle the parsing.

USING
Include json.h and link with json.cpp
Everything is under jrb_json.

The central type is jrb_json::value.

An int, std::string can be directly assigned.
To use an array you can initialize an value like this
value val = jrb_json::array(1,2,"string");

To use an object you can initialize a value like this
value val = jrb_json::object("name1","value1")("name2","value2");

a value that has been initialized with array has the following members
push_back
operator [](int)
size()

an value that has been initialized with object has the following members
operator [] (std::string)
size()

To convert to and from a json string use the following functions

	value parse_json(const std::string& str) 

	std::string json_string(const value& d);
	std::string pretty_json_string(const value& d);


LICENSE

jrb json is released under the BOOST SOFTWARE LICENSE
rapidjson is licensed under a MIT license