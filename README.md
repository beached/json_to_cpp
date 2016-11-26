# JSON to C++
This program will take either a json file or a URL to a web service and build C++ classes to work with that data.  By default it will create the serialization linkage for the JsonLink library that is part of https://github.com/beached/parse_json .

* boost::optional is used for optional members(those that are not always there or are null in some cases
* std::string is used for strings
* int64_t is used for integral types
* double is used for real types
* bool is used for boolean types
* classes are given the name of their member suffixed with a "_t"

# Requirements
* CMake >= 2.8.12 https://cmake.org/
* Boost >= 1.58 https://www.boost.org/
* Curl https://curl.haxx.se/

# Testing
Currently it is known to build on clang 3.8.1 and gcc 6.2.0

Both Boost and Curl are often packaged within a Linux distribution and available for Windows/Mac

# Building
The build system uses cmake.  Often a build can be accomplished by creating a build folder in the project source folder and typing 
```
cmake ..
cmake --build .
```
# Running
To output the C++ code to the terminal one just needs to type ```json_to_cpp_bin --in_file jsonfile.json```

```
Command line options
Options:
  --help                                print option descriptions
  --in_file arg                         json source file path or url
  --cpp_file arg                        output c++ file
  --header_file arg                     output c++ header file.  If not 
                                        specified uses cpp_file
  --use_jsonlink arg (=1)               Use JsonLink serializaion/deserializati
                                        on
  --user_agent arg (=Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.100 Safari/537.36)
                                        User agent to use when downloading via 
                                        URL
```
# Example 
## H2 JSON Data
```
{
    "glossary": {
        "title": "example glossary",
        "GlossDiv": {
            "title": "S",
            "GlossList": {
                "GlossEntry": {
                    "ID": "SGML",
                    "SortAs": "SGML",
                    "GlossTerm": "Standard Generalized Markup Language",
                    "Acronym": "SGML",
                    "Abbrev": "ISO 8879:1986",
                    "GlossDef": {
                        "para": "A meta-markup language, used to create markup languages such as DocBook.",
                        "GlossSeeAlso": ["GML", "XML"]
                    },  
                    "GlossSee": "markup"
                }   
            }   
        }   
    }   
}
```
## Generated C++ Code
```
#include <string>
#include <vector>
#include <daw/json/daw_json_link.h>

struct GlossDef_t: public daw::json::JsonLink<GlossDef_t> {
	std::vector<std::string> GlossSeeAlso;
	std::string para;

	GlossDef_t( );
	GlossDef_t( GlossDef_t const & other );
	GlossDef_t( GlossDef_t && other );
	~GlossDef_t( );

	GlossDef_t & operator=( GlossDef_t const & ) = default;
	GlossDef_t & operator=( GlossDef_t && ) = default;
private:
	void set_links( );
};	// GlossDef_t

struct GlossEntry_t: public daw::json::JsonLink<GlossEntry_t> {
	std::string Abbrev;
	std::string Acronym;
	GlossDef_t GlossDef;
	std::string GlossSee;
	std::string GlossTerm;
	std::string ID;
	std::string SortAs;

	GlossEntry_t( );
	GlossEntry_t( GlossEntry_t const & other );
	GlossEntry_t( GlossEntry_t && other );
	~GlossEntry_t( );

	GlossEntry_t & operator=( GlossEntry_t const & ) = default;
	GlossEntry_t & operator=( GlossEntry_t && ) = default;
private:
	void set_links( );
};	// GlossEntry_t

struct GlossList_t: public daw::json::JsonLink<GlossList_t> {
	GlossEntry_t GlossEntry;

	GlossList_t( );
	GlossList_t( GlossList_t const & other );
	GlossList_t( GlossList_t && other );
	~GlossList_t( );

	GlossList_t & operator=( GlossList_t const & ) = default;
	GlossList_t & operator=( GlossList_t && ) = default;
private:
	void set_links( );
};	// GlossList_t

struct GlossDiv_t: public daw::json::JsonLink<GlossDiv_t> {
	GlossList_t GlossList;
	std::string title;

	GlossDiv_t( );
	GlossDiv_t( GlossDiv_t const & other );
	GlossDiv_t( GlossDiv_t && other );
	~GlossDiv_t( );

	GlossDiv_t & operator=( GlossDiv_t const & ) = default;
	GlossDiv_t & operator=( GlossDiv_t && ) = default;
private:
	void set_links( );
};	// GlossDiv_t

struct glossary_t: public daw::json::JsonLink<glossary_t> {
	GlossDiv_t GlossDiv;
	std::string title;

	glossary_t( );
	glossary_t( glossary_t const & other );
	glossary_t( glossary_t && other );
	~glossary_t( );

	glossary_t & operator=( glossary_t const & ) = default;
	glossary_t & operator=( glossary_t && ) = default;
private:
	void set_links( );
};	// glossary_t

struct root_object_t: public daw::json::JsonLink<root_object_t> {
	glossary_t glossary;

	root_object_t( );
	root_object_t( root_object_t const & other );
	root_object_t( root_object_t && other );
	~root_object_t( );

	root_object_t & operator=( root_object_t const & ) = default;
	root_object_t & operator=( root_object_t && ) = default;
private:
	void set_links( );
};	// root_object_t

GlossDef_t::GlossDef_t( ):
		daw::json::JsonLink<GlossDef_t>{ },
		GlossSeeAlso{ },
		para{ } {

	set_links( );
}

GlossDef_t::GlossDef_t( GlossDef_t const & other ):
		daw::json::JsonLink<GlossDef_t>{ },
		GlossSeeAlso{ other.GlossSeeAlso },
		para{ other.para } {

	set_links( );
}

GlossDef_t::GlossDef_t( GlossDef_t && other ):
		daw::json::JsonLink<GlossDef_t>{ },
		GlossSeeAlso{ std::move( other.GlossSeeAlso ) },
		para{ std::move( other.para ) } {

	set_links( );
}

GlossDef_t::~GlossDef_t( ) { }

void GlossDef_t::set_links( ) {
	link_array( "GlossSeeAlso", GlossSeeAlso );
	link_string( "para", para );
}

GlossEntry_t::GlossEntry_t( ):
		daw::json::JsonLink<GlossEntry_t>{ },
		Abbrev{ },
		Acronym{ },
		GlossDef{ },
		GlossSee{ },
		GlossTerm{ },
		ID{ },
		SortAs{ } {

	set_links( );
}

GlossEntry_t::GlossEntry_t( GlossEntry_t const & other ):
		daw::json::JsonLink<GlossEntry_t>{ },
		Abbrev{ other.Abbrev },
		Acronym{ other.Acronym },
		GlossDef{ other.GlossDef },
		GlossSee{ other.GlossSee },
		GlossTerm{ other.GlossTerm },
		ID{ other.ID },
		SortAs{ other.SortAs } {

	set_links( );
}

GlossEntry_t::GlossEntry_t( GlossEntry_t && other ):
		daw::json::JsonLink<GlossEntry_t>{ },
		Abbrev{ std::move( other.Abbrev ) },
		Acronym{ std::move( other.Acronym ) },
		GlossDef{ std::move( other.GlossDef ) },
		GlossSee{ std::move( other.GlossSee ) },
		GlossTerm{ std::move( other.GlossTerm ) },
		ID{ std::move( other.ID ) },
		SortAs{ std::move( other.SortAs ) } {

	set_links( );
}

GlossEntry_t::~GlossEntry_t( ) { }

void GlossEntry_t::set_links( ) {
	link_string( "Abbrev", Abbrev );
	link_string( "Acronym", Acronym );
	link_object( "GlossDef", GlossDef );
	link_string( "GlossSee", GlossSee );
	link_string( "GlossTerm", GlossTerm );
	link_string( "ID", ID );
	link_string( "SortAs", SortAs );
}

GlossList_t::GlossList_t( ):
		daw::json::JsonLink<GlossList_t>{ },
		GlossEntry{ } {

	set_links( );
}

GlossList_t::GlossList_t( GlossList_t const & other ):
		daw::json::JsonLink<GlossList_t>{ },
		GlossEntry{ other.GlossEntry } {

	set_links( );
}

GlossList_t::GlossList_t( GlossList_t && other ):
		daw::json::JsonLink<GlossList_t>{ },
		GlossEntry{ std::move( other.GlossEntry ) } {

	set_links( );
}

GlossList_t::~GlossList_t( ) { }

void GlossList_t::set_links( ) {
	link_object( "GlossEntry", GlossEntry );
}

GlossDiv_t::GlossDiv_t( ):
		daw::json::JsonLink<GlossDiv_t>{ },
		GlossList{ },
		title{ } {

	set_links( );
}

GlossDiv_t::GlossDiv_t( GlossDiv_t const & other ):
		daw::json::JsonLink<GlossDiv_t>{ },
		GlossList{ other.GlossList },
		title{ other.title } {

	set_links( );
}

GlossDiv_t::GlossDiv_t( GlossDiv_t && other ):
		daw::json::JsonLink<GlossDiv_t>{ },
		GlossList{ std::move( other.GlossList ) },
		title{ std::move( other.title ) } {

	set_links( );
}

GlossDiv_t::~GlossDiv_t( ) { }

void GlossDiv_t::set_links( ) {
	link_object( "GlossList", GlossList );
	link_string( "title", title );
}

glossary_t::glossary_t( ):
		daw::json::JsonLink<glossary_t>{ },
		GlossDiv{ },
		title{ } {

	set_links( );
}

glossary_t::glossary_t( glossary_t const & other ):
		daw::json::JsonLink<glossary_t>{ },
		GlossDiv{ other.GlossDiv },
		title{ other.title } {

	set_links( );
}

glossary_t::glossary_t( glossary_t && other ):
		daw::json::JsonLink<glossary_t>{ },
		GlossDiv{ std::move( other.GlossDiv ) },
		title{ std::move( other.title ) } {

	set_links( );
}

glossary_t::~glossary_t( ) { }

void glossary_t::set_links( ) {
	link_object( "GlossDiv", GlossDiv );
	link_string( "title", title );
}

root_object_t::root_object_t( ):
		daw::json::JsonLink<root_object_t>{ },
		glossary{ } {

	set_links( );
}

root_object_t::root_object_t( root_object_t const & other ):
		daw::json::JsonLink<root_object_t>{ },
		glossary{ other.glossary } {

	set_links( );
}

root_object_t::root_object_t( root_object_t && other ):
		daw::json::JsonLink<root_object_t>{ },
		glossary{ std::move( other.glossary ) } {

	set_links( );
}

root_object_t::~root_object_t( ) { }

void root_object_t::set_links( ) {
	link_object( "glossary", glossary );
}
```
