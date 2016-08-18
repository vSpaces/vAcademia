#ifndef     __FTGDITextureGlyph__
#define     __FTGDITextureGlyph__

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "FTGL.h"
#include "FTTextureGlyph.h"

/**
* FTTextureGlyph is a specialisation of FTGlyph for creating texture
* glyphs.
* 
* @see FTGlyphContainer
*
*/
class FTGL_EXPORT FTTextureGDIGlyph : public FTGlyph
{
public:
	FTTextureGDIGlyph( FT_GlyphSlot glyph, int characterId, int id, int xOffset, int yOffset, GLsizei width, GLsizei height);
	virtual ~FTTextureGDIGlyph();

	/**
	* Renders this glyph at the current pen position.
	*
	* @param pen   The current pen position.
	* @return      The advance distance for this glyph.
	*/
	virtual const FTPoint& Render( const FTPoint& pen);

	/**
	* Reset the currently active texture to zero to get into a known state before
	* drawing a string. This is to get round possible threading issues.
	*/
	static void FTTextureGDIGlyph::ResetActiveTexture(){ activeTextureID = 0;}

protected:
	/**
	* The width of the glyph 'image'
	*/
	int destWidth;

	/**
	* The height of the glyph 'image'
	*/
	int destHeight;

	/**
	* Vector from the pen position to the topleft corner of the pixmap
	*/
	FTPoint pos;

	/**
	* The texture co-ords of this glyph within the texture.
	*/
	FTPoint uv[2];

	/**
	* The texture index that this glyph is contained in.
	*/
	int glTextureID;

	/**
	* The texture index of the currently active texture
	*
	* We keep track of the currently active texture to try to reduce the number
	* of texture bind operations.
	*/
	static GLint activeTextureID;
};

#endif