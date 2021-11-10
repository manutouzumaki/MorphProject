#ifndef MAPEDITOR_H
#define MAPEDITOR_H

struct tilesheet
{
    texture *Texture;
    i32 TexWidth;
    i32 TexHeight;
    i32 TileWidth;
    i32 TileHeight;
    v2 Position;
};

struct editor
{
    tilesheet TileSheet;
    tilemap Tilemap;
    u32 BrushValue;
    bool MouseOnUI;
};

void InitEditor(editor *Editor, texture *Texture, u32 TileWidth, u32 TileHeight)
{
    // Create tilesheet
    tilesheet TileSheet = {};
    texture_info TextureInfo = GetTextureInfo(Texture);
    TileSheet.Texture = Texture;
    TileSheet.TexWidth = TextureInfo.Width;
    TileSheet.TexHeight = TextureInfo.Height; 
    TileSheet.TileWidth = TileWidth;
    TileSheet.TileHeight = TileHeight;
    // Load Editor
    Editor->TileSheet = TileSheet;
    Editor->Tilemap.TileWidth = TileWidth;
    Editor->Tilemap.TileHeight = TileHeight;
    Editor->Tilemap.Rows = TILEMAP_ROWS;
    Editor->Tilemap.Cols = TILEMAP_COLS;
}

bool PointOnQuad(i32 X, i32 Y, i32 Width, i32 Height,
                       i32 XPos, i32 YPos)
{
    if(XPos >= X && XPos <= X + Width &&
       YPos >= Y && YPos <= Y + Height)
    {
        return true;
    }
    return false;
}

void SetBrushValue(input *Input, editor *Editor)
{
    tilesheet *TileSheet = &Editor->TileSheet;
    if(PointOnQuad(TileSheet->Position.X, TileSheet->Position.Y,
                   TileSheet->TexWidth, TileSheet->TexHeight,
                   Input->MouseX, Input->MouseY))
    {
        Editor->MouseOnUI = true;
        if(Input->MouseButtons->Left.IsDown != Input->MouseButtons->Left.WasDown)
        {
            if(Input->MouseButtons->Left.IsDown)
            {

                i32 XFrame = (i32)((Input->MouseX - TileSheet->Position.X) / TileSheet->TileWidth);
                i32 YFrame = (i32)((Input->MouseY - TileSheet->Position.Y) / TileSheet->TileHeight);
                i32 NumberOfCols = TileSheet->TexWidth / TileSheet->TileWidth;
                Editor->BrushValue = YFrame * NumberOfCols + XFrame;
            }
        }
    }
    else
    { 
        Editor->MouseOnUI = false;
    }
}

void PaintTilemap(input *Input, editor *Editor, v3 CamPosition)
{
    tilemap *Tilemap = &Editor->Tilemap;
    if(!Editor->MouseOnUI)
    {
        if(Input->MouseButtons->Left.IsDown)
        {
            // Adjust the camera...
            CamPosition.X -= WND_WIDTH*0.5f;
            CamPosition.Y -= WND_HEIGHT*0.5f;
            i32 MouseX = Input->MouseX + CamPosition.X;
            i32 MouseY = Input->MouseY + CamPosition.Y;

            if(PointOnQuad(0, 0, Tilemap->Cols*Tilemap->TileWidth, Tilemap->Rows*Tilemap->TileHeight,
                        MouseX, MouseY))
            {
                i32 XFrame = (i32)(MouseX / Tilemap->TileWidth);
                i32 YFrame = (i32)(MouseY / Tilemap->TileHeight);
                u32 Index = YFrame * Tilemap->Cols + XFrame;
                Tilemap->Tiles[Index].Base = Editor->BrushValue; 
            }
        }
    }
}

#endif
