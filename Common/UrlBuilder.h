#ifndef CURLBUILDER_H
#define CURLBUILDER_H

/*
* Creates a new string URL object from given params
* must be in the form:
*
* <protocol>://<host>[:<port>]/<resource>[#<bookmark>][?<parameters>]
*
* If a parsing error occurs, fullURL is set to an empty string ("").
*
* @param url the URL
*/

class CUrlBuilder
{
public:
	CUrlBuilder();
	virtual ~CUrlBuilder();

	// return location string
	std::wstring	toString();

	// set URL params
	//void	setLocation( wchar_t* apwcLocationName);
	void	setCoords( float ax, float ay, float az);
	void	setRotations( float ara, float arx, float ary, float arz);
	void	setReality( unsigned int auRealityId);

protected:
	float x, y, z;
	float ra, rx, ry, rz;
	unsigned int realityId;
	//std::wstring locationName;

	bool coordsIsSet, realityIdIsSet, locationNameIsSet, rotationsIsSet;
};

#endif // CURLPARSER_H
