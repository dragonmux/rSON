# rSON 0.1.0

The project was conceived after surveying the C/C++ based JSON implementations for something that was lightweight and easy to use (and by that, I mean requires no setup code and simply works to produce output that is easy to access the structure of and grab data out from) and comming up very short.

## The Idea

rSON implements a configuration-less JSON complient parser which does literally all the donkey work. an example call works like:
	JSONAtom *rootNode = rSON::parseJSON("[\"My test array\", \"With multiple elements\", True]");

It also implements a set of classes that form the following tree structure to build trees out of the results of parsing:

 *	JSONAtom
	*	JSONNull
	*	JSONBool
	*	JSONInt
	*	JSONFloat
	*	JSONString
	*	JSONArray
	*	JSONObject

Internally the tree is very strongly typed, throwing exceptions that you can catch when you attempt an invalid conversion, but due to limitations in C++ itself has to present the entire tree as a set of JSONAtom's.
To do the job of converting from those JSONAtom pointers/references, JSONAtom contains a set of as*() functions which perform the type conversions. Although mildly inconvinient, this still provides nice semantics in my honest opinion.

## The API

The following should be everything you need to know without reading the header for imediately using rSON in your programs and using it effectively:

JSONAtom implements the following conversion functions:

 *	`void *asNull()`
 *	`bool asBool()`
 *	`int asInt()`
 *	`double asFloat()` - The return type for this is forced by the numerical requirements of the standard
 *	`const char *asString()` - do not call `free()` or `delete` on this as cleanup is automatic when you delete the root JSONAtom
 *	`JSONString &asStringRef()`
 *	`JSONArray *asArray()` and `JSONArray &asArrayRef()`
 *	`JSONObject *asObject()` and `JSONObject &asObjectRef()`
 
The last three sets of functions are the odd ones out in that they convert between rSON types rather than directly to C++ types for use directly in your programs.
This is done to allow rSON to provide a nicer interface as JSONArray and JSONObject implement various operator overrides to allow transparent lookup of JSONAtom's held by each via either numerical index (JSONArray) or associative style string index (JSONObject)
This is also done to allow direct access to a JSONString object for std::string-like use

The JSONAtom also fully supports indexing if the real type it represents would also - so for JSONArray and JSONObject nodes, no manual conversion is required for indexing into complex trees (as of 0.1.0)
This further allows elegant code through rSON in user programs.

rSON attempts (and I hope succeeds) to provide a fully Associative Array style interface to JSONObject. I have to implement `->get()` methods though for when this is too bulky or doesn't fit nicely when using rSON.

There are a couple of exceptions that will get thrown either if you feed rSON with data that's not actually JSON, or if you try to convert to a type but the JSONAtom is not that type.
These exceptions are:

 *	JSONParserError
 *	JSONTypeError

They both provide an error() function for displaying a message of what was wrong, and JSONParserError provides way to get the raw error constants if you wish to write your own messages.
The other exceptions, although possible, are not disuessed here as they are involved in the lookup systems for JSONArray and JSONObject and should only arrise from the JSON not being what was expected during data extraction.

## The License

As stated in the code, I have licensed the library using LGPL v3+.
Please report bugs to dx-mon@users.sourceforge.net

## Known Bugs

None
