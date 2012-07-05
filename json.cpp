//  Copyright John R. Bandela 2012
//
// Distributed under the Boost Software License, Version 1.0.
//    (See  http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include "External/rapidjson/reader.h"
#include "External/rapidjson/rapidjson.h"
#include "External/rapidjson/writer.h"
#include "External/rapidjson/prettywriter.h"
#include <stack>
#include "json.h"
#include <deque>
#include <algorithm>

namespace {

	struct string_reader{
		typedef char Ch;        //!< Character type. Only support char.

		string_reader(const std::string& str) : str_(&str),pos_(0){}
		char Peek() const {
			if(pos_ >= str_->size()) return 0;
			else return str_->at(pos_); 
		}
		char Take() { return str_->at(pos_++); }
		size_t Tell() const { return pos_; }

		// not implemented
		void Put(char c) { ; }

		// Not implemented
		char* PutBegin() { return 0; }
		size_t PutEnd(char*) { return 0; }

		const std::string* str_;
		size_t pos_;
	};


	struct string_writer{
		typedef char Ch;        //!< Character type. Only support char.

		string_writer() : pos_(0){}

		void Put(char c) {str_.push_back(c) ; }

		// Not implemented
		char* PutBegin() { return 0; }
		size_t PutEnd(char*) { return 0; }

		std::string str_;
		size_t pos_;
	};
	struct parse_handler {
		typedef char Ch;
		typedef size_t SizeType;

		parse_handler(){}

		void Null(){
			st_.push(jrb_json::value());
		}
		void Bool(bool b){
			st_.push(jrb_json::value(b));
		}
		void Int(int i){
			st_.push(jrb_json::value(i));
		}
		void Uint(unsigned i){
			st_.push(jrb_json::value(i));
		}
		void Int64(int64_t i){
			st_.push(jrb_json::value(i));
		}
		void Uint64(uint64_t i){
			st_.push(jrb_json::value(i));
		}
		void Double(double d){
			st_.push(jrb_json::value(d));
		}
		void String(const Ch* str, SizeType length, bool copy){
			st_.push(jrb_json::value(std::string(str, str+length)));

		}
		void StartObject(){

		}
		void EndObject(SizeType memberCount){
			jrb_json::value obj = jrb_json::object();
			for(int i = 0; i<memberCount; i++){
				check_st();
				jrb_json::value val = st_.top();
				st_.pop();
				check_st();
				std::string name = st_.top().get_exact<std::string>();
				obj[name] = val;
				st_.pop();
			}
			st_.push(obj);
		}
		void StartArray(){

		}
		void EndArray(SizeType elementCount){
			jrb_json::value v = jrb_json::array();
			for(int i = 0; i < elementCount; i++){
				check_st();
				v.push_back(st_.top());
				st_.pop();
			}
			auto& vec = v.get_exact<jrb_json::array_type>();
			std::reverse(vec.begin(),vec.end());
			st_.push(v);

		}

		std::stack<jrb_json::value> st_;

		void check_st(){
			if(!st_.size()){
				throw jrb_json::json_parse_error();
			}
		}
	};
	template<class Writer = rapidjson::Writer<string_writer>>
	struct print_visitor:public boost::static_visitor<>{

		print_visitor(Writer* w):jw_(w){}

		template<class U>
		void operator()(U t) 
		{

		}	

		void operator()(int t) 
		{
			jw_->Int(t);
		}	
		void operator()(boost::int64_t t) 
		{
			jw_->Int64(t);
		}	
		void operator()(unsigned int t) 
		{
			jw_->Uint(t);
		}	
		void operator()(boost::uint64_t t) 
		{
			jw_->Uint64(t);
		}	
		void operator()(double t) 
		{
			jw_->Double(t);
		}
		void operator()(bool b)
		{
			jw_->Bool(b);
		}
		void operator()(const std::string& str) 
		{
			jw_->String(str.c_str());
		}
		void operator()(const jrb_json::object_type& o) 
		{
			jw_->StartObject();
			for(auto& p:o){
				jw_->String(p.first.c_str());
				boost::apply_visitor(*this,p.second.get().jrb_get_rep());
			}
			jw_->EndObject(o.size());

		}
		void operator()(const jrb_json::array_type& a) 
		{
			jw_->StartArray();
			for(auto& p:a){
				boost::apply_visitor(*this,p.get().jrb_get_rep());
			}
			jw_->EndArray(a.size());    
		}
		void operator()(const jrb_json::null_t&) 
		{
			jw_->Null();
		}
		Writer*  jw_;

	};

}

namespace jrb_json{
	value parse_json(const std::string& str){

		rapidjson::Reader r;
		parse_handler h;

		string_reader stream(str);
		if(r.Parse<0>(stream,h)){
			h.check_st();
			return h.st_.top();
		}
		else{
			throw json_parse_error();
		}


	}

	std::string json_string(const value& d){
		string_writer sw;
		rapidjson::Writer<string_writer> w(sw);
		print_visitor<rapidjson::Writer<string_writer>> v(&w);
		boost::apply_visitor(v,d.jrb_get_rep());
		return sw.str_;
	}
	std::string pretty_json_string(const value& d){
		string_writer sw;
		rapidjson::PrettyWriter<string_writer> w(sw);
		print_visitor<rapidjson::PrettyWriter<string_writer>> v(&w);
		boost::apply_visitor(v,d.jrb_get_rep());
		return sw.str_;
	}


}