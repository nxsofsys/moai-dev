// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <zl-util/ZLColor.h>
#include <zl-util/ZLFloat.h>
#include <zl-util/ZLInterpolate.h>

#define WR 0.299f
#define WG 0.587f
#define WB 0.114f

#define U_MAX 0.436f
#define V_MAX 0.615f

//================================================================//
// ZLColor
//================================================================//

//----------------------------------------------------------------//
u32 ZLColor::Average ( u32 c0, u32 c1 ) {
	
	u32 r = (( c0 & 0xFF ) + ( c1 & 0xFF )) >> 1;
	u32 g = ((( c0 >> 0x08 ) & 0xFF ) + (( c1 >> 0x08 ) & 0xFF )) >> 1;
	u32 b = ((( c0 >> 0x10 ) & 0xFF ) + (( c1 >> 0x10 ) & 0xFF )) >> 1;
	u32 a = ((( c0 >> 0x18 ) & 0xFF ) + (( c1 >> 0x18 ) & 0xFF )) >> 1;
	
	return r + ( g << 0x08 ) + ( b << 0x10 ) + ( a << 0x18 );
}

//----------------------------------------------------------------//
u32 ZLColor::Average ( u32 c0, u32 c1, u32 c2, u32 c3 ) {
	
	u32 r = (( c0 & 0xFF ) + ( c1 & 0xFF ) + ( c2 & 0xFF ) + ( c3 & 0xFF )) >> 2;
	u32 g = ((( c0 >> 0x08 ) & 0xFF ) + (( c1 >> 0x08 ) & 0xFF ) + (( c2 >> 0x08 ) & 0xFF ) + (( c3 >> 0x08 ) & 0xFF )) >> 2;
	u32 b = ((( c0 >> 0x10 ) & 0xFF ) + (( c1 >> 0x10 ) & 0xFF ) + (( c2 >> 0x10 ) & 0xFF ) + (( c3 >> 0x10 ) & 0xFF )) >> 2;
	u32 a = ((( c0 >> 0x18 ) & 0xFF ) + (( c1 >> 0x18 ) & 0xFF ) + (( c2 >> 0x18 ) & 0xFF ) + (( c3 >> 0x18 ) & 0xFF )) >> 2;
	
	return r + ( g << 0x08 ) + ( b << 0x10 ) + ( a << 0x18 );
}

//----------------------------------------------------------------//
u32 ZLColor::BilerpFixed ( u32 c0, u32 c1, u32 c2, u32 c3, u8 xt, u8 yt ) {

	u32 s0 = ZLColor::LerpFixed ( c0, c1, xt );
	u32 s1 = ZLColor::LerpFixed ( c2, c3, xt );
	
	return ZLColor::LerpFixed ( s0, s1, yt );
}

//----------------------------------------------------------------//
void ZLColor::Convert ( void* dest, Format destFmt, const void* src, Format srcFmt, u32 nColors ) {

	static const u32 MAX_COLORS = 2048;

	u32 buffer [ MAX_COLORS ];
	u32* bufferPtr = buffer;
	u32 color;
	
	while ( nColors ) {
	
		u32 copy = nColors;
		if ( copy > MAX_COLORS ) {
			copy = MAX_COLORS;
		}
		nColors -= copy;
	
		switch ( srcFmt ) {
			
			case A_8:
			
				for ( u32 i = 0; i < copy; ++i ) {
				
					color = *( u8* )src;
					src = ( void* )(( size_t )src + 1 );
					
					buffer [ i ] = color << 0x18;
				}
				bufferPtr = buffer;
				break;
			
			case RGB_888:
				
				for ( u32 i = 0; i < copy; ++i ) {
					
					color = *( u32* )src;
					src = ( void* )(( size_t )src + 3 );
					
					buffer [ i ]= color | 0xff000000;
				}
				bufferPtr = buffer;
				break;
				
			case RGB_565:
				
				for ( u32 i = 0; i < copy; ++i ) {
					
					color = *( u16* )src;
					src = ( void* )(( size_t )src + 2 );
					
					buffer [ i ] =	((( color >> 0x00 ) & 0x1F ) << 0x03 ) +
									((( color >> 0x05 ) & 0x3F ) << 0x02 ) +
									((( color >> 0x0B ) & 0x1F ) << 0x03 ) +
									0xff000000;
					
				}
				bufferPtr = buffer;
				break;
			
			case RGBA_5551: 
				
				for ( u32 i = 0; i < copy; ++i ) {
				
					color = *( u16* )src;
					src = ( void* )(( size_t )src + 2 );
					
					buffer [ i ] =	((( color >> 0x00 ) & 0x1F ) << 0x03 ) +
									((( color >> 0x05 ) & 0x1F ) << 0x0B ) +
									((( color >> 0x0A ) & 0x1F ) << 0x13 ) +
									(((( color >> 0x0F ) & 0xff ) ? 0xff : 0x00 ) << 0x18 );
				}
				bufferPtr = buffer;
				break;

			case RGBA_4444:
			
				for ( u32 i = 0; i < copy; ++i ) {
				
					color = *( u32* )src;
					src = ( void* )(( size_t )src + 2 );
					
					buffer [ i ] =	((( color >> 0x00 ) & 0x0F ) << 0x04 ) +
									((( color >> 0x04 ) & 0x0F ) << 0x0C ) +
									((( color >> 0x08 ) & 0x0F ) << 0x14 ) +
									((( color >> 0x0C ) & 0x0F ) << 0x1C );
				}
				bufferPtr = buffer;
				break;

			case RGBA_8888:
				bufferPtr = ( u32* )src;
				break;
			
			default:
				return;
		}
		
		switch ( destFmt ) {
		
			case A_8:
				
				for ( u32 i = 0; i < copy; ++i ) {
				
					color = bufferPtr [ i ];
					
					(( u8* )dest )[ 0 ] = ( color >> 0x18 ) & 0xFF;
					dest = ( void* )(( size_t )dest + 1 );
				}
				break;
		
			case RGB_888:
			
				for ( u32 i = 0; i < copy; ++i ) {
				
					color = bufferPtr [ i ];
					
					(( u8* )dest )[ 0 ] = color & 0xFF;
					(( u8* )dest )[ 1 ] = ( color >> 8 ) & 0xFF;
					(( u8* )dest )[ 2 ] = ( color >> 16 ) & 0xFF;
					dest = ( void* )(( size_t )dest + 3 );
				}
				break;
				
			case RGB_565:
			
				for ( u32 i = 0; i < copy; ++i ) {
				
					color = bufferPtr [ i ];
					
					*( u16* )dest =	((( color >> 0x03 ) & 0x1F ) << 0x0B ) +
									((( color >> 0x0A ) & 0x3F ) << 0x05 ) +
									((( color >> 0x13 ) & 0x1F ) << 0x00 );
					dest = ( void* )(( size_t )dest + 2 );
				}
				break;
						
			case RGBA_5551: 
				
				for ( u32 i = 0; i < copy; ++i ) {
				
					color = bufferPtr [ i ];
					
					*( u16* )dest =		((( color >> 0x03 ) & 0x1F ) << 0x00 ) +
										((( color >> 0x0B ) & 0x1F ) << 0x05 ) +
										((( color >> 0x13 ) & 0x1F ) << 0x0A ) +
										(((( color >> 0x1C ) & 0x0F ) ? 0x01 : 0x00 ) << 0x0F );
					dest = ( void* )(( size_t )dest + 2 );
				}
				break;

			case RGBA_4444:
				
				for ( u32 i = 0; i < copy; ++i ) {
				
					color = bufferPtr [ i ];
					
					*( u16* )dest =		((( color >> 0x04 ) & 0x0F ) << 0x0C ) +
										((( color >> 0x0C ) & 0x0F ) << 0x08 ) +
										((( color >> 0x14 ) & 0x0F ) << 0x04 ) +
										((( color >> 0x1C ) & 0x0F ) << 0x00 );
					dest = ( void* )(( size_t )dest + 2 );
				}
				break;

			case RGBA_8888:
				
				memcpy ( dest, bufferPtr, copy * sizeof ( u32 ));
				dest = ( void* )(( size_t )dest + ( copy * sizeof ( u32 )));
				break;
			
			default:
				break;
		}
	}
}

//----------------------------------------------------------------//
u32 ZLColor::ConvertFromRGBA ( u32 color, Format format ) {

	switch ( format ) {
		
		case A_8:
			return ( color >> 0x18 ) & 0x000000FF;
		
		case RGB_888:
			return color & 0x00FFFFFF;
			
		case RGB_565:
		
			return	((( color >> 0x03 ) & 0x1F ) << 0x0B ) +
					((( color >> 0x0A ) & 0x3F ) << 0x05 ) +
					((( color >> 0x13 ) & 0x1F ) << 0x00 );
					
		case RGBA_5551: 
			
			return	((( color >> 0x03 ) & 0x1F ) << 0x00 ) +
					((( color >> 0x0B ) & 0x1F ) << 0x05 ) +
					((( color >> 0x13 ) & 0x1F ) << 0x0A ) +
					(((( color >> 0x1C ) & 0x0F ) ? 0x01 : 0x00 ) << 0x0F );

		case RGBA_4444:
			
			return	((( color >> 0x04 ) & 0x0F ) << 0x0C ) +
					((( color >> 0x0C ) & 0x0F ) << 0x08 ) +
					((( color >> 0x14 ) & 0x0F ) << 0x04 ) +
					((( color >> 0x1C ) & 0x0F ) << 0x00 );

		case RGBA_8888:
			return color;
		
		default:
			break;
	}
	
	return 0;
}

//----------------------------------------------------------------//
u32 ZLColor::ConvertToRGBA ( u32 color, Format format ) {

	switch ( format ) {
		
		case A_8:
			return ( color << 0x18 ) & 0xFF000000;
		
		case RGB_888:
			return color | 0xFF000000;
			
		case RGB_565:
		
			return	((( color >> 0x00 ) & 0x1F ) << 0x03 ) +
					((( color >> 0x05 ) & 0x3F ) << 0x02 ) +
					((( color >> 0x0B ) & 0x1F ) << 0x03 ) +
					0xff000000;
					
		case RGBA_5551: 
			
			return	((( color >> 0x00 ) & 0x1F ) << 0x03 ) +
					((( color >> 0x05 ) & 0x1F ) << 0x0B ) +
					((( color >> 0x0A ) & 0x1F ) << 0x13 ) +
					(((( color >> 0x0F ) & 0xff ) ? 0xff : 0x00 ) << 0x18 );

		case RGBA_4444:
		
			return	((( color >> 0x00 ) & 0x0F ) << 0x04 ) +
					((( color >> 0x04 ) & 0x0F ) << 0x0C ) +
					((( color >> 0x08 ) & 0x0F ) << 0x14 ) +
					((( color >> 0x0C ) & 0x0F ) << 0x1C );

		case RGBA_8888:
			return color;
		
		default:
			break;
	}
	
	return 0;
}

//----------------------------------------------------------------//
u32 ZLColor::GetDepth ( Format format ) {

	switch ( format ) {
		case A_8:			return 8;
		case RGB_888:		return 24;
		case RGB_565:		return 16;
		case RGBA_5551:		return 16;
		case RGBA_4444:		return 16;
		case RGBA_8888:		return 32;
		default:			break;
	}
	return 0;
}

//----------------------------------------------------------------//
u32 ZLColor::GetMask ( Format format ) {

	switch ( format ) {
		case A_8:			return 0x000000FF;
		case RGB_888:		return 0x00ffffff;
		case RGB_565:		return 0x0000ffff;
		case RGBA_5551:		return 0x0000ffff;
		case RGBA_4444:		return 0x0000ffff;
		case RGBA_8888:		return 0xffffffff;
		default:			break;
	}
	return 0;
}

//----------------------------------------------------------------//
u32 ZLColor::GetSize ( Format format ) {

	switch ( format ) {
		case A_8:			return 1;
		case RGB_888:		return 3;
		case RGB_565:		return 2;
		case RGBA_5551:		return 2;
		case RGBA_4444:		return 2;
		case RGBA_8888:		return 4;
		default:			break;
	}
	return 0;
}

//----------------------------------------------------------------//
u32 ZLColor::LerpFixed ( u32 c0, u32 c1, u8 t ) {
	
	u32 r0 = ( c0 ) & 0xFF;
	u32 g0 = ( c0 >> 0x08 ) & 0xFF;
	u32 b0 = ( c0 >> 0x10 ) & 0xFF;
	u32 a0 = ( c0 >> 0x18 ) & 0xFF;
	
	u32 r1 = ( c1 ) & 0xFF;
	u32 g1 = ( c1 >> 0x08 ) & 0xFF;
	u32 b1 = ( c1 >> 0x10 ) & 0xFF;
	u32 a1 = ( c1 >> 0x18 ) & 0xFF;
	
	u32 r = r0 + ((( r1 - r0 ) * t ) >> 0x08 );
	u32 g = g0 + ((( g1 - g0 ) * t ) >> 0x08 );
	u32 b = b0 + ((( b1 - b0 ) * t ) >> 0x08 );
	u32 a = a0 + ((( a1 - a0 ) * t ) >> 0x08 );
	
	return r + ( g << 0x08 ) + ( b << 0x10 ) + ( a << 0x18 );
}

//----------------------------------------------------------------//
u32 ZLColor::NearestNeighbor ( u32 c0, u32 c1, u32 c2, u32 c3, u8 xt, u8 yt ) {

	if ( xt < 128 ) {
		if ( yt < 128 ) {
			return c0;
		}
		return c2;
	}
	if ( yt < 128 ) {
		return c1;
	}
	return c3;
}

//----------------------------------------------------------------//
u32 ZLColor::PackRGBA ( int r, int g, int b, int a ) {

	return	( r << 0x00 ) +
			( g << 0x08 ) +
			( b << 0x10 ) +
			( a << 0x18 );
}

//----------------------------------------------------------------//
u32 ZLColor::PackRGBA ( float r, float g, float b, float a ) {

	return	(( u8 )( r * 255.0f ) << 0x00) +
			(( u8 )( g * 255.0f ) << 0x08 ) +
			(( u8 )( b * 255.0f ) << 0x10 ) +
			(( u8 )( a * 255.0f ) << 0x18 );
}

//----------------------------------------------------------------//
void ZLColor::PremultiplyAlpha ( void* colors, Format format, u32 nColors ) {

	u32 color;
	u32 alpha;
	
	switch ( format ) {
		
		case A_8:
			break;
		
		case RGB_888:
			break;
			
		case RGB_565:
			break;
		
		case RGBA_5551: 
			
			for ( u32 i = 0; i < nColors; ++i ) {
				color = *( u16* )colors;
				alpha = ( color >> 0x0F ) & 0x01;
				*( u16* )colors = ( u16 )(	(((( color >> 0x00 ) & 0x1F ) * alpha ) << 0x00 ) +
											(((( color >> 0x05 ) & 0x1F ) * alpha ) << 0x05 ) +
											(((( color >> 0x0A ) & 0x1F ) * alpha ) << 0x0A ) +
											( alpha << 0x0F ));
				colors = ( void* )(( size_t )colors + 2 );
			}
			break;

		case RGBA_4444:
		
			for ( u32 i = 0; i < nColors; ++i ) {
				color = *( u16* )colors;
				alpha = color & 0x0F;
				*( u16* )colors = ( u16 )(	alpha +
											((((( color >> 0x04 ) & 0x0F ) * alpha ) >> 0x04 ) << 0x04 ) +
											((((( color >> 0x08 ) & 0x0F ) * alpha ) >> 0x04 ) << 0x08 ) +
											((((( color >> 0x0c ) & 0x0F ) * alpha ) >> 0x04 ) << 0x0C ));
				colors = ( void* )(( size_t )colors + 2 );
			}
			break;

		case RGBA_8888:
		
			for ( u32 i = 0; i < nColors; ++i ) {
				color = *( u32* )colors;
				alpha = ( color >> 0x18 ) & 0xFF;
				*( u32* )colors =	((((( color >> 0x00 ) & 0xFF ) * alpha ) >> 0x08 ) << 0x00 ) +
									((((( color >> 0x08 ) & 0xFF ) * alpha ) >> 0x08 ) << 0x08 ) +
									((((( color >> 0x10 ) & 0xFF ) * alpha ) >> 0x08 ) << 0x10 ) +
									( alpha << 0x18 );
				colors = ( void* )(( size_t )colors + 4 );
			}
			break;
		
		default:
			break;
	}
}

//----------------------------------------------------------------//
u32 ZLColor::ReadRGBA ( const void* stream, Format format ) {

	u32 size = ZLColor::GetSize ( format );
	return ZLColor::ConvertToRGBA ( USPixel::ReadPixel ( stream, size ), format );
}

//----------------------------------------------------------------//
ZLColorVec ZLColor::Set ( u32 c0 ) {
	
	ZLColorVec color (
		(( c0 & ( 255 << 0x00 )) >> 0x00 ) / 255.0f,
		(( c0 & ( 255 << 0x08 )) >> 0x08 ) / 255.0f,
		(( c0 & ( 255 << 0x10 )) >> 0x10 ) / 255.0f,
		(( c0 & ( 255 << 0x18 )) >> 0x18 ) / 255.0f );
	
	return color;
}

//----------------------------------------------------------------//
void ZLColor::WriteRGBA ( void* stream, u32 color, Format format ) {

	u32 size = ZLColor::GetSize ( format );
	USPixel::WritePixel ( stream, ZLColor::ConvertFromRGBA ( color, format ), size );
}

//================================================================//
// USPixel
//================================================================//

//----------------------------------------------------------------//
u32 USPixel::GetDepth ( Format format, ZLColor::Format colorFormat ) {

	switch ( format ) {
		case TRUECOLOR:		return ZLColor::GetDepth ( colorFormat );
		case INDEX_4:		return 4;
		case INDEX_8:		return 8;
		default:			break;
	}
	return 0;
}

//----------------------------------------------------------------//
u32 USPixel::GetMask ( Format format, ZLColor::Format colorFormat ) {

	switch ( format ) {
		case TRUECOLOR:		return ZLColor::GetMask ( colorFormat );
		case INDEX_4:		return 0x0000000F;
		case INDEX_8:		return 0x000000FF;
		default:			break;
	}
	return 0;
}

//----------------------------------------------------------------//
u32 USPixel::GetPaletteCount ( Format format ) {

	switch ( format ) {
		case TRUECOLOR:		return 0;
		case INDEX_4:		return 16;
		case INDEX_8:		return 256;
		default:			break;
	}
	return 0;
}

//----------------------------------------------------------------//
u32 USPixel::GetPaletteSize ( Format format, ZLColor::Format colorFormat ) {

	return ( USPixel::GetPaletteCount ( format ) * ZLColor::GetSize ( colorFormat ));
}

//----------------------------------------------------------------//
float USPixel::GetSize ( Format format, ZLColor::Format colorFormat ) {

	switch ( format ) {
		case TRUECOLOR:		return ( float )ZLColor::GetSize ( colorFormat );
		case INDEX_4:		return 0.5f;
		case INDEX_8:		return 1.0f;
		default:			break;
	}
	return 0;
}

//----------------------------------------------------------------//
u32 USPixel::ReadPixel ( const void* stream, u32 nBytes ) {

	const u8* bytes = ( const u8* )stream;
	u32 pixel = 0;
	u32 shift = 0;

	switch ( nBytes ) {
		
		case 4:
			pixel += ( *( bytes++ )) << shift;
			shift += 0x08;
		case 3:
			pixel += ( *( bytes++ )) << shift;
			shift += 0x08;
		case 2:
			pixel += ( *( bytes++ )) << shift;
			shift += 0x08;
		case 1:
			pixel += ( *( bytes++ )) << shift;
	}
	
	return pixel;
}

//----------------------------------------------------------------//
void USPixel::ToTrueColor ( void* destColors, const void* srcColors, const void* palette, u32 nColors, ZLColor::Format colorFormat, Format pixelFormat ) {

	u32 colorSize = ZLColor::GetDepth ( colorFormat );

	if ( pixelFormat == TRUECOLOR ) {
		memcpy ( destColors, srcColors, nColors * colorSize );
	}

	for ( u32 i = 0; i < nColors; ++i ) {
	
		u32 index = 0;
	
		if ( pixelFormat == INDEX_4 ) {
			index = *( const u8* )(( size_t )srcColors + ( i >> 1 ));
			index = ( i & 1 ) ? ( index >> 4 ) : index;
			index = index & 0xff;
		}
		else {
			index = *( const u8* )(( size_t )srcColors + i );
		}
		
		memcpy ( destColors, ( const void* )(( size_t )palette + ( colorSize * index )), colorSize );
		destColors = ( void* )(( size_t )destColors + colorSize );
	}
}

//----------------------------------------------------------------//
void USPixel::WritePixel ( void* stream, u32 pixel, u32 nBytes ) {

	u8* bytes = ( u8* )stream;
	u32 shift = 0;

	switch ( nBytes ) {

		case 4:
			*( bytes++ ) = ( pixel >> shift ) & 0xff;
			shift += 0x08;
		case 3:
			*( bytes++ ) = ( pixel >> shift ) & 0xff;
			shift += 0x08;
		case 2:
			*( bytes++ ) = ( pixel >> shift ) & 0xff;
			shift += 0x08;
		case 1:
			*( bytes++ ) = ( pixel >> shift ) & 0xff;
	}
}

//================================================================//
// ZLColorVec
//================================================================//

//----------------------------------------------------------------//
void ZLColorVec::Add ( const ZLColorVec& c ) {

	this->mR += c.mR;
	this->mG += c.mG;
	this->mB += c.mB;
	this->mA += c.mA;
}

//----------------------------------------------------------------//
void ZLColorVec::FromHSV ( float h, float s, float v ) {
	
	if( s == 0.0f ) {
		this->mR = v;
		this->mG = v;
		this->mB = v;
		return;
	}
	
	h /= 60.0f;
	
	int i = ( int )floor ( h );
	float f = h - ( float )i;
	
	float p = v * ( 1.0f - s );
	float q = v * ( 1.0f - s * f );
	float t = v * ( 1.0f - s * ( 1.0f - f ) );
	
	switch ( i ) {
	
		case 0:
			this->mR = v;
			this->mG = t;
			this->mB = p;
			break;
			
		case 1:
			this->mR = q;
			this->mG = v;
			this->mB = p;
			break;
			
		case 2:
			this->mR = p;
			this->mG = v;
			this->mB = t;
			break;
			
		case 3:
			this->mR = p;
			this->mG = q;
			this->mB = v;
			break;
			
		case 4:
			this->mR = t;
			this->mG = p;
			this->mB = v;
			break;
			
		case 5:
		default:
			this->mR = v;
			this->mG = p;
			this->mB = q;
			break;
	}
}

//----------------------------------------------------------------//
void ZLColorVec::FromYUV ( float y, float u, float v ) {

	this->mR = y + ( v * (( 1.0f - WR ) / V_MAX ));
	this->mG = y - ( u * (( WB * ( 1.0f - WB )) / ( U_MAX * WG ))) - ( v * (( WR * ( 1.0f - WR )) / ( V_MAX * WG )));
	this->mB = y + ( u * (( 1.0f - WB ) / U_MAX ));
}

//----------------------------------------------------------------//
float ZLColorVec::GetLuma () const {

	return ( WR * this->mR ) + ( WG * this->mG ) + ( WB * this->mB );
}

//----------------------------------------------------------------//
bool ZLColorVec::IsClear () {

	return (( this->mR == 0.0f ) && ( this->mG == 0.0f ) && ( this->mB == 0.0f ) && ( this->mA == 0.0f ));
}

//----------------------------------------------------------------//
void ZLColorVec::Lerp ( u32 mode, const ZLColorVec& v0, const ZLColorVec& v1, float t ) {

	this->mR = ZLInterpolate::Interpolate ( mode, v0.mR, v1.mR, t );
	this->mG = ZLInterpolate::Interpolate ( mode, v0.mG, v1.mG, t );
	this->mB = ZLInterpolate::Interpolate ( mode, v0.mB, v1.mB, t );
	this->mA = ZLInterpolate::Interpolate ( mode, v0.mA, v1.mA, t );
}

//----------------------------------------------------------------//
void ZLColorVec::Modulate ( const ZLColorVec& v0 ) {

	this->mR = this->mR * v0.mR;
	this->mG = this->mG * v0.mG;
	this->mB = this->mB * v0.mB;
	this->mA = this->mA * v0.mA;
}

//----------------------------------------------------------------//
u32 ZLColorVec::PackRGBA () const {

	return ZLColor::PackRGBA ( this->mR, this->mG, this->mB, this->mA );
}

//----------------------------------------------------------------//
void ZLColorVec::SetRGBA ( u32 color ) {

	this->mR = ( float )(( color ) & 0xff ) / 255.0f;
	this->mG = ( float )(( color >> 8 ) & 0xff ) / 255.0f;
	this->mB = ( float )(( color >> 16 ) & 0xff ) / 255.0f;
	this->mA = ( float )(( color >> 24 ) & 0xff ) / 255.0f;
}

//----------------------------------------------------------------//
void ZLColorVec::Set ( float r, float g, float b, float a ) {

	this->mR = r;
	this->mG = g;
	this->mB = b;
	this->mA = a;
}

//----------------------------------------------------------------//
void ZLColorVec::SetBlack () {

	this->Set ( 0.0f, 0.0f, 0.0f, 1.0f );
}

//----------------------------------------------------------------//
void ZLColorVec::SetWhite () {

	this->Set ( 1.0f, 1.0f, 1.0f, 1.0f );
}

//----------------------------------------------------------------//
void ZLColorVec::ToHSV ( float& h, float& s, float& v ) {

	float r = this->mR;
	float g = this->mG;
	float b = this->mB;

	float min = ZLFloat::Min ( r, g, b );
	float max = ZLFloat::Min ( r, g, b );
	float delta = max - min;
	
	v = max;
	
	if ( max != 0.0f ) {
		s = delta / max;
	}
	else {
		// r = g = b = 0
		// s = 0, v is undefined
		s = 0.0f;
		h = -1.0f;
		return;
	}
	
	if ( r == max ) {
		h = ( g - b ) / delta; // between yellow & magenta
	}
	else if ( g == max ) {
		h = 2.0f + ( b - r ) / delta; // between cyan & yellow
	}
	else {
		h = 4.0f + ( r - g ) / delta; // between magenta & cyan
	}

	h *= 60.0f; // degrees
	
	if ( h < 0.0f ) {
		h += 360.0f;
	}
}

//----------------------------------------------------------------//
void ZLColorVec::ToYUV ( float& y, float& u, float& v ) {
	
	y = ( WR * this->mR ) + ( WG * this->mG ) + ( WB * this->mB );
	u = U_MAX * (( this->mB - y ) / ( 1.0f - WB ));
	v = V_MAX * (( this->mR - y ) / ( 1.0f - WR ));
}

//----------------------------------------------------------------//
ZLColorVec::ZLColorVec () {
}

//----------------------------------------------------------------//
ZLColorVec::ZLColorVec ( float r, float g, float b, float a ) {

	this->mR = r;
	this->mG = g;
	this->mB = b;
	this->mA = a;
}
