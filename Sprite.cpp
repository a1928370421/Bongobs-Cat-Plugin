#include "Sprite.hpp"
#include "VtuberDelegate.hpp"

Sprite::Sprite(float x, float y, float width, float height,
		       GLuint textureId, GLuint programId)
	: _rect()
{
	_rect.left = (x - width);
	_rect.right = (x + width );
	_rect.up = (y + height );
	_rect.down = (y - height );
	_textureId = textureId;

	// 何番目のattribute変数か
	_positionLocation = glGetAttribLocation(programId, "position");
	_uvLocation = glGetAttribLocation(programId, "uv");
	_textureLocation = glGetUniformLocation(programId, "texture");
	_colorLocation = glGetUniformLocation(programId, "baseColor");

	_spriteColor[0] = 1.0f;
	_spriteColor[1] = 1.0f;
	_spriteColor[2] = 1.0f;
	_spriteColor[3] = 1.0f;

	_width = width;
	_height = height;
}

Sprite::~Sprite() {}

void Sprite::Render(int i) const
{
	
	// 画面サイズを取得する
	int maxWidth, maxHeight;
	maxWidth=VtuberDelegate::GetInstance()->getBufferWidth(i);
	maxHeight=VtuberDelegate::GetInstance()->getBufferHeight(i);

	if (maxWidth == 0 || maxHeight == 0) {
		return; // この際は描画できず
	}

	//mix texture
	glEnable(GL_BLEND);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);

	glEnable(GL_TEXTURE_2D);
	const GLfloat uvVertex[] = {
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
	};

	// attribute属性を有効にする
	glEnableVertexAttribArray(_positionLocation);
	glEnableVertexAttribArray(_uvLocation);

	// uniform属性の登録
	glUniform1i(_textureLocation, 0);

	// 頂点データ
	float positionVertex[] = {
		(_rect.right - maxWidth * 0.5f) / (maxWidth * 0.5f),
		(_rect.up - maxHeight * 0.5f) / (maxHeight * 0.5f),
		(_rect.left - maxWidth * 0.5f) / (maxWidth * 0.5f),
		(_rect.up - maxHeight * 0.5f) / (maxHeight * 0.5f),
		(_rect.left - maxWidth * 0.5f) / (maxWidth * 0.5f),
		(_rect.down - maxHeight * 0.5f) / (maxHeight * 0.5f),
		(_rect.right - maxWidth * 0.5f) / (maxWidth * 0.5f),
		(_rect.down - maxHeight * 0.5f) / (maxHeight * 0.5f)};

	// attribute属性を登録
	glVertexAttribPointer(_positionLocation, 2, GL_FLOAT, false, 0, positionVertex);
	glVertexAttribPointer(_uvLocation, 2, GL_FLOAT, false, 0, uvVertex);

	glUniform4f(_colorLocation, _spriteColor[0], _spriteColor[1],
		    _spriteColor[2], _spriteColor[3]);

	// モデルの描画
	glBindTexture(GL_TEXTURE_2D, _textureId);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Sprite::RenderImmidiate(int id,GLuint textureId,
				 const GLfloat uvVertex[8]) const
{
	// 画面サイズを取得する
	int maxWidth, maxHeight;
	maxWidth = VtuberDelegate::GetInstance()->getBufferWidth(id);
	maxHeight = VtuberDelegate::GetInstance()->getBufferHeight(id);

	if (maxWidth == 0 || maxHeight == 0) {
		return; // この際は描画できず
	}

	glEnable(GL_TEXTURE_2D);

	// attribute属性を有効にする
	glEnableVertexAttribArray(_positionLocation);
	glEnableVertexAttribArray(_uvLocation);

	// uniform属性の登録
	glUniform1i(_textureLocation, 0);

	// 頂点データ
	float positionVertex[] = {
		(_rect.right - maxWidth * 0.5f) / (maxWidth * 0.5f),
		(_rect.up - maxHeight * 0.5f) / (maxHeight * 0.5f),
		(_rect.left - maxWidth * 0.5f) / (maxWidth * 0.5f),
		(_rect.up - maxHeight * 0.5f) / (maxHeight * 0.5f),
		(_rect.left - maxWidth * 0.5f) / (maxWidth * 0.5f),
		(_rect.down - maxHeight * 0.5f) / (maxHeight * 0.5f),
		(_rect.right - maxWidth * 0.5f) / (maxWidth * 0.5f),
		(_rect.down - maxHeight * 0.5f) / (maxHeight * 0.5f)};

	// attribute属性を登録
	glVertexAttribPointer(_positionLocation, 2, GL_FLOAT, false, 0,
			      positionVertex);
	glVertexAttribPointer(_uvLocation, 2, GL_FLOAT, false, 0, uvVertex);

	glUniform4f(_colorLocation, _spriteColor[0], _spriteColor[1],
		    _spriteColor[2], _spriteColor[3]);

	// モデルの描画
	glBindTexture(GL_TEXTURE_2D, textureId);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Sprite::SetColor(float r, float g, float b, float a)
{
	_spriteColor[0] = r;
	_spriteColor[1] = g;
	_spriteColor[2] = b;
	_spriteColor[3] = a;
}

void Sprite::ResetRect(float x, float y, float width, float height)
{
	_rect.left = (x - width * 0.5f);
	_rect.right = (x + width * 0.5f);
	_rect.up = (y + height * 0.5f);
	_rect.down = (y - height * 0.5f);

	_width = width;
	_height = height;
}

void Sprite::ResetLocation(float x, float y) {

	_rect.left = (x - _width * 0.5f);
	_rect.right = (x + _width * 0.5f);
	_rect.up = (y + _height * 0.5f);
	_rect.down = (y - _height * 0.5f);

}
