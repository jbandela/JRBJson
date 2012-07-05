//  Copyright John R. Bandela 2012
//
// Distributed under the Boost Software License, Version 1.0.
//    (See  http://www.boost.org/LICENSE_1_0.txt)

// Class value modeled on facebook folly::dynamic but uses boost and compiles under MSVC 11
#pragma once

#ifndef JRB_JSON_03_07_2012
#define JRB_JSON_03_07_2012

#include<string>
#include <boost/variant.hpp>
#include <vector>
#include <map>
#include <memory>
#include <boost/cstdint.hpp>
#include <utility>
#include <stdexcept>
#include <boost/lexical_cast.hpp>

namespace jrb_json{

	struct value;

	typedef std::map<std::string,boost::recursive_wrapper<value> > object_type;
	typedef std::vector<boost::recursive_wrapper<value> > array_type;
	struct null_t{};
	typedef boost::variant<int,unsigned int,boost::int64_t,boost::uint64_t,double, std::string,bool,object_type,array_type,null_t> value_holder;

	struct null_error:public std::runtime_error{
		null_error():std::runtime_error("Null value"){}
	};

	struct invalid_type_error:public std::runtime_error{
		invalid_type_error():std::runtime_error("Invalid type"){}
	};
	struct json_parse_error:public std::runtime_error{
		json_parse_error():std::runtime_error("parse error"){}
	};



	inline null_t null(){return null_t();}

	struct object_holder{
		object_type& get(){return o_;}
		operator object_type&(){return get();}
		const object_type& get()const{return o_;}
		operator const object_type&()const{return get();}

		object_type& operator ()(const std::string& name, const value& d){
			o_[name] = d;
			return o_;
		}
	private:
		object_type o_;

	};

	struct value{

		value(){assign(null_t());}

		template<class T>
		value(const T& t){assign(t);}

		value(const char* s){assign(std::string(s));}

		value(const object_holder& v){assign(v);}
		value(null_t (*)()){assign(null_t());}

		value(const value& other){assign(*other.ptr_.get());}
		value(value&& other){ptr_ = std::move(other.ptr_);}

		value& operator=(const value& other){assign(*other.ptr_.get()); return *this;}
		value& operator=(value&& other){ptr_ = std::move(other.ptr_); return *this;}

		value& operator=(null_t (*)()){assign(null_t()); return *this;}

		template<class T>
		T& get_exact(){
			T* ret = boost::get<T>(ptr_.get());
			if(ret){return *ret;}
			else{throw invalid_type_error();}
		}	
		template<class T>
		const T& get_exact()const {
			T* ret = boost::get<T>(ptr_.get());
			if(ret){return *ret;}
			else{throw invalid_type_error();}
		}

		template<class T>
		T get()const{
			get_visitor<T> visitor;
			boost::apply_visitor(visitor,*ptr_);
			return visitor.val_;
		}

		template<class T>
		bool has()const{
			T* ret = boost::get<T>(ptr_.get());
			if(ret) return true;
			else return false;
		}

		value& operator[](int i){
			return get_exact<array_type>().at(i).get();
		}	
		const value& operator[](int i)const{
			return get_exact<array_type>().at(i).get();
		}
		value& operator[](const std::string& name){
			return get_exact<object_type>()[name].get();
		}	
		const value& operator[](const std::string& name)const{
			auto& o = get_exact<object_type>();
			auto iter = o.find(name);
			if(iter != o.end()){
				return iter->second.get();
			}
			else{
				throw std::out_of_range("value[name] not found");
			}
		}

		void push_back(const value& d)
		{
			get_exact<array_type>().push_back(d);

		}
		int size()const{
			if(has<null_t>()) return 0;
			if(has<array_type>()) return get_exact<array_type>().size();
			if(has<object_type>()) return get_exact<object_type>().size();
			else return 1;
		}

		value& operator[](const char* name){return (*this)[std::string(name)];}
		const value& operator[](const char* name)const{return (*this)[std::string(name)];}



		value_holder& jrb_get_rep(){return *ptr_;}
		const value_holder& jrb_get_rep()const{return *ptr_;}

	private:
		std::unique_ptr<value_holder> ptr_;
		template<class T>
		void assign(const T& t){ptr_.reset(new value_holder(t));}
		void assign(const object_holder& o){ptr_.reset(new value_holder(o.get()));}
		void assign(null_t n){ptr_.reset(new value_holder(null_t()));}


		template<class Type>
		struct get_visitor:public boost::static_visitor<>{

			template<class U>
			void operator()(U t) 
			{

			}	

			void operator()(int t) 
			{
				val_ = boost::numeric_cast<Type>(t);
			}	
			void operator()(boost::int64_t t) 
			{
				val_ = boost::numeric_cast<Type>(t);
			}	
			void operator()(unsigned int t) 
			{
				val_ = boost::numeric_cast<Type>(t);
			}	
			void operator()(boost::uint64_t t) 
			{
				val_ = boost::numeric_cast<Type>(t);
			}	
			void operator()(double t) 
			{
				val_ = boost::numeric_cast<Type>(t);
			}
			void operator()(bool b)
			{
				val_ = b;
			}
			void operator()(const std::string& str) 
			{
				val_ = boost::lexical_cast<Type>(str);
			}
			void operator()(const object_type&) 
			{
				throw invalid_type_error();
			}
			void operator()(const array_type&) 
			{
				throw invalid_type_error();
			}
			void operator()(const null_t&) 
			{
				throw null_error();
			}
			Type val_;

		};
		template<>
		struct get_visitor<std::string>:public boost::static_visitor<>{

			template<class U>
			void operator()(U t) 
			{

			}	
			void operator()(int t) 
			{
				val_ = boost::lexical_cast<std::string>(t);
			}	
			void operator()(boost::int64_t t) 
			{
				val_ = boost::lexical_cast<std::string>(t);
			}	
			void operator()(unsigned int t) 
			{
				val_ = boost::lexical_cast<std::string>(t);
			}	
			void operator()(boost::uint64_t t) 
			{
				val_ = boost::lexical_cast<std::string>(t);
			}	
			void operator()(double t) 
			{
				val_ = boost::lexical_cast<std::string>(t);
			}
			void operator()(bool b)
			{
				if(b) val_ = "true";
				else val_ = "false";


			}
			void operator()(const std::string& str) 
			{
				val_ = str;
			}
			void operator()(const object_type&) 
			{
				val_ = "object";
			}
			void operator()(const array_type&) 
			{
				val_ = "array";
			}
			void operator()(const null_t&) 
			{
				val_ = "null";
			}
			std::string val_;

		};





	};

	value parse_json(const std::string& str);

	std::string json_string(const value& d);
	std::string pretty_json_string(const value& d);

	inline object_holder object(){
		object_holder ret;
		return ret;
	}

	inline object_holder object(const std::string& name, const value& d){
		object_holder ret;
		ret(name,d);
		return ret;
	}

	inline array_type array(){
		return array_type();
	}
	inline array_type array(const value& d0){
		array_type ret;
		ret.push_back(d0);
		return ret;
	}
	inline array_type array(const value& d0,const value& d1){
		array_type ret;
		ret.push_back(d0);
		ret.push_back(d1);
		return ret;
	}
	inline array_type array(const value& d0,const value& d1,const value& d2){
		array_type ret;
		ret.push_back(d0);
		ret.push_back(d1);
		ret.push_back(d2);
		return ret;
	}
	inline  array_type array(const value& d0,const value& d1,const value& d2,const value& d3){
		array_type ret;
		ret.push_back(d0);
		ret.push_back(d1);
		ret.push_back(d2);
		ret.push_back(d3);
		return ret;
	}  
	inline array_type array(const value& d0,const value& d1,const value& d2,const value& d3,const value& d4){
		array_type ret;
		ret.push_back(d0);
		ret.push_back(d1);
		ret.push_back(d2);
		ret.push_back(d3);
		ret.push_back(d4);
		return ret;
	}
	inline array_type array(const value& d0,const value& d1,const value& d2,const value& d3,const value& d4,const value& d5){
		array_type ret;
		ret.push_back(d0);
		ret.push_back(d1);
		ret.push_back(d2);
		ret.push_back(d3);
		ret.push_back(d4);
		ret.push_back(d5);
		return ret;
	}

	inline array_type array(const value& d0,const value& d1,const value& d2,const value& d3,const value& d4,const value& d5,const value& d6){
		array_type ret;
		ret.push_back(d0);
		ret.push_back(d1);
		ret.push_back(d2);
		ret.push_back(d3);
		ret.push_back(d4);
		ret.push_back(d5);
		ret.push_back(d6);
		return ret;
	}
	inline array_type array(const value& d0,const value& d1,const value& d2,const value& d3,const value& d4,const value& d5,const value& d6,const value& d7){
		array_type ret;
		ret.push_back(d0);
		ret.push_back(d1);
		ret.push_back(d2);
		ret.push_back(d3);
		ret.push_back(d4);
		ret.push_back(d5);
		ret.push_back(d6);
		ret.push_back(d7);
		return ret;
	}
	inline array_type array(const value& d0,const value& d1,const value& d2,const value& d3,const value& d4,const value& d5,const value& d6,const value& d7,const value& d8){
		array_type ret;
		ret.push_back(d0);
		ret.push_back(d1);
		ret.push_back(d2);
		ret.push_back(d3);
		ret.push_back(d4);
		ret.push_back(d5);
		ret.push_back(d6);
		ret.push_back(d7);
		ret.push_back(d8);
		return ret;
	}
	inline array_type array(const value& d0,const value& d1,const value& d2,const value& d3,const value& d4,const value& d5,const value& d6,const value& d7,const value& d8,const value& d9){
		array_type ret;
		ret.push_back(d0);
		ret.push_back(d1);
		ret.push_back(d2);
		ret.push_back(d3);
		ret.push_back(d4);
		ret.push_back(d5);
		ret.push_back(d6);
		ret.push_back(d7);
		ret.push_back(d8);
		ret.push_back(d9);
		return ret;
	}
	inline array_type array(const value& d0,const value& d1,const value& d2,const value& d3,const value& d4,const value& d5,const value& d6,const value& d7,const value& d8,const value& d9,const value& d10){
		array_type ret;
		ret.push_back(d0);
		ret.push_back(d1);
		ret.push_back(d2);
		ret.push_back(d3);
		ret.push_back(d4);
		ret.push_back(d5);
		ret.push_back(d6);
		ret.push_back(d7);
		ret.push_back(d8);
		ret.push_back(d9);
		ret.push_back(d10);
		return ret;
	}
	inline array_type array(const value& d0,const value& d1,const value& d2,const value& d3,const value& d4,const value& d5,const value& d6,const value& d7,const value& d8,const value& d9,const value& d10,const value& d11){
		array_type ret;
		ret.push_back(d0);
		ret.push_back(d1);
		ret.push_back(d2);
		ret.push_back(d3);
		ret.push_back(d4);
		ret.push_back(d5);
		ret.push_back(d6);
		ret.push_back(d7);
		ret.push_back(d8);
		ret.push_back(d9);
		ret.push_back(d10);
		ret.push_back(d11);
		return ret;
	}
	inline array_type array(const value& d0,const value& d1,const value& d2,const value& d3,const value& d4,const value& d5,const value& d6,const value& d7,const value& d8,const value& d9,
		const value& d10,const value& d11,const value& d12){
			array_type ret;
			ret.push_back(d0);
			ret.push_back(d1);
			ret.push_back(d2);
			ret.push_back(d3);
			ret.push_back(d4);
			ret.push_back(d5);
			ret.push_back(d6);
			ret.push_back(d7);
			ret.push_back(d8);
			ret.push_back(d9);
			ret.push_back(d10);
			ret.push_back(d11);
			ret.push_back(d12);
			return ret;
	}
	inline array_type array(const value& d0,const value& d1,const value& d2,const value& d3,const value& d4,const value& d5,const value& d6,const value& d7,const value& d8,const value& d9,
		const value& d10,const value& d11,const value& d12,const value& d13){
			array_type ret;
			ret.push_back(d0);
			ret.push_back(d1);
			ret.push_back(d2);
			ret.push_back(d3);
			ret.push_back(d4);
			ret.push_back(d5);
			ret.push_back(d6);
			ret.push_back(d7);
			ret.push_back(d8);
			ret.push_back(d9);
			ret.push_back(d10);
			ret.push_back(d11);
			ret.push_back(d12);
			ret.push_back(d13);
			return ret;
	}
	inline array_type array(const value& d0,const value& d1,const value& d2,const value& d3,const value& d4,const value& d5,const value& d6,const value& d7,const value& d8,const value& d9,
		const value& d10,const value& d11,const value& d12,const value& d13,const value& d14){
			array_type ret;
			ret.push_back(d0);
			ret.push_back(d1);
			ret.push_back(d2);
			ret.push_back(d3);
			ret.push_back(d4);
			ret.push_back(d5);
			ret.push_back(d6);
			ret.push_back(d7);
			ret.push_back(d8);
			ret.push_back(d9);
			ret.push_back(d10);
			ret.push_back(d11);
			ret.push_back(d12);
			ret.push_back(d13);
			ret.push_back(d14);
			return ret;
	}

	inline array_type array(const value& d0,const value& d1,const value& d2,const value& d3,const value& d4,const value& d5,const value& d6,const value& d7,const value& d8,const value& d9,
		const value& d10,const value& d11,const value& d12,const value& d13,const value& d14,const value& d15){
			array_type ret;
			ret.push_back(d0);
			ret.push_back(d1);
			ret.push_back(d2);
			ret.push_back(d3);
			ret.push_back(d4);
			ret.push_back(d5);
			ret.push_back(d6);
			ret.push_back(d7);
			ret.push_back(d8);
			ret.push_back(d9);
			ret.push_back(d10);
			ret.push_back(d11);
			ret.push_back(d12);
			ret.push_back(d13);
			ret.push_back(d14);
			ret.push_back(d15);
			return ret;
	}

}

#endif