# JSON to C++
This program will take either a json file or a URL to a web service and build C++ classes to work with that data.  By default it will create the serialization linkage for the JsonLink library that is part of https://github.com/beached/daw_json_link .

* std::optional is used for optional members(those that are not always there or are null in some cases
* std::string is used for strings
* int64_t is used for integral types
* double is used for real types
* bool is used for boolean types
* classes are given the name of their member suffixed with a "_t"
* identifier names are filtered such that C++ keywords, empty id's, or all number id's are prefixed with _json
* Any character in an id that isn't A-Za-z0-9 or _ will be escaped via 0xXXX

# Requirements
* CMake >= 2.8.12 https://cmake.org/
* Boost >= 1.58 https://www.boost.org/
* Curl https://m_curl.haxx.se/

Both Boost and Curl are often packaged within a Linux distribution and available for Windows/Mac

# Building
The build system uses cmake.  Often a build can be accomplished by creating a build folder in the project source folder and typing  

```
mkdir build
cd build
cmake ..
cmake --build .
```
# Running
To output the C++ code to the terminal one just needs to type ```json_to_cpp_bin --in_file jsonfile.json``` or for a url something like ```json_to_cpp_bin --in_file http://ip.jsontest.com/```

```  
Command line options
Options:
  --help                                print option descriptions
  --in_file arg                         json source file path or url
  --kv_paths arg                        Specify class members that are key
                                        value pairs
  --use_jsonlink arg (=1)               Use JsonLink serializaion/deserializati
                                        on
  --output_file arg                     output goes to c++ header file.
  --allow_overwrite arg (=0)            Overwrite existing output files
  --hide_null_only arg (=1)             Do not output json entries that are
                                        only ever null
  --use_string_view arg (=0)            Use std::string_view instead of
                                        std::string.  Must ensure buffer is
                                        available after parsing when this is
                                        used
  --root_object arg (=root_object)      Name of the nameless root object
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
## Generated C++ Code by calling ```json_to_cpp --in_file name.json```
```
#include <tuple>
#include <string>
#include <vector>
#include <daw/json/daw_json_link.h>

struct GlossDef_t {
	std::string para;
	std::vector<std::string> GlossSeeAlso;
};	// GlossDef_t

inline auto describe_json_class( GlossDef_t ) {
	using namespace daw::json;
	static constexpr char const para[] = "para";
	static constexpr char const GlossSeeAlso[] = "GlossSeeAlso";
	return daw::json::class_description_t<
		json_string<para>
		,json_array<GlossSeeAlso, std::vector<std::string>, json_string<no_name>>
>{};
}

inline auto to_json_data( GlossDef_t const & value ) {
	return std::forward_as_tuple( value.para, value.GlossSeeAlso );
}

struct GlossEntry_t {
	std::string ID;
	std::string SortAs;
	std::string GlossTerm;
	std::string Acronym;
	std::string Abbrev;
	GlossDef_t GlossDef;
	std::string GlossSee;
};	// GlossEntry_t

inline auto describe_json_class( GlossEntry_t ) {
	using namespace daw::json;
	static constexpr char const ID[] = "ID";
	static constexpr char const SortAs[] = "SortAs";
	static constexpr char const GlossTerm[] = "GlossTerm";
	static constexpr char const Acronym[] = "Acronym";
	static constexpr char const Abbrev[] = "Abbrev";
	static constexpr char const GlossDef[] = "GlossDef";
	static constexpr char const GlossSee[] = "GlossSee";
	return daw::json::class_description_t<
		json_string<ID>
		,json_string<SortAs>
		,json_string<GlossTerm>
		,json_string<Acronym>
		,json_string<Abbrev>
		,json_class<GlossDef, GlossDef_t>
		,json_string<GlossSee>
>{};
}

inline auto to_json_data( GlossEntry_t const & value ) {
	return std::forward_as_tuple( value.ID, value.SortAs, value.GlossTerm, value.Acronym, value.Abbrev, value.GlossDef, value.GlossSee );
}

struct GlossList_t {
	GlossEntry_t GlossEntry;
};	// GlossList_t

inline auto describe_json_class( GlossList_t ) {
	using namespace daw::json;
	static constexpr char const GlossEntry[] = "GlossEntry";
	return daw::json::class_description_t<
		json_class<GlossEntry, GlossEntry_t>
>{};
}

inline auto to_json_data( GlossList_t const & value ) {
	return std::forward_as_tuple( value.GlossEntry );
}

struct GlossDiv_t {
	std::string title;
	GlossList_t GlossList;
};	// GlossDiv_t

inline auto describe_json_class( GlossDiv_t ) {
	using namespace daw::json;
	static constexpr char const title[] = "title";
	static constexpr char const GlossList[] = "GlossList";
	return daw::json::class_description_t<
		json_string<title>
		,json_class<GlossList, GlossList_t>
>{};
}

inline auto to_json_data( GlossDiv_t const & value ) {
	return std::forward_as_tuple( value.title, value.GlossList );
}

struct glossary_t {
	std::string title;
	GlossDiv_t GlossDiv;
};	// glossary_t

inline auto describe_json_class( glossary_t ) {
	using namespace daw::json;
	static constexpr char const title[] = "title";
	static constexpr char const GlossDiv[] = "GlossDiv";
	return daw::json::class_description_t<
		json_string<title>
		,json_class<GlossDiv, GlossDiv_t>
>{};
}

inline auto to_json_data( glossary_t const & value ) {
	return std::forward_as_tuple( value.title, value.GlossDiv );
}

struct root_object_t {
	glossary_t glossary;
};	// root_object_t

inline auto describe_json_class( root_object_t ) {
	using namespace daw::json;
	static constexpr char const glossary[] = "glossary";
	return daw::json::class_description_t<
		json_class<glossary, glossary_t>
>{};
}

inline auto to_json_data( root_object_t const & value ) {
	return std::forward_as_tuple( value.glossary );
}
```
