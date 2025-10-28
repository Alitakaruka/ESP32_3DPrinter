#pragma once
////////////////////////////////////////////////////////////////////
class AnyScreen
{
private:

public:
    AnyScreen() = default;
    virtual void DrawRect(int X0,int Y0,int X1,int Y1, bool fill) = 0;
    virtual void DrawLine(int X0,int Y0,int X1,int Y1) = 0;
    virtual void SetPixel(int X,int Y, bool state) = 0;
};
////////////////////////////////////////////////////////////////////
class Point
{
private:
    uint32_t X = 0;
    uint32_t Y = 0; 
public:
    Point() = default;
    Point(uint32_t X, uint32_t Y);
    void SetXY(uint32_t x,uint32_t y){X = x;Y = y;}
    uint32_t GetX(){return X;}
    uint32_t GetY(){return Y;} 
};
////////////////////////////////////////////////////////////////////

//Widgets
////////////////////////////////////////////////////////////////////
class Widget
{
private:
protected:
    bool dirty = true;
    bool visible = true;
    uint8_t borderPX = 1;

public:
    virtual void Draw(AnyScreen& screen) = 0;
};

class Window : public Widget
{
private:
    Widget* child = nullptr;
    Point Position;
    int X,Y,W,H;
public:
};

class Label : public Widget
{
private:
    char Header[20];
    char Text[50];
public:
    void set_Header(const char* str){
        if(strlen(str) >= 20){
            return;
        }
        strncpy(Header, str,20);
        this->dirty = true;
    }
    void set_Text(const char* str){
        if(strlen(str) >= 50){
            return;
        }
        strncpy(this->Header, str,50);
        this->dirty = true;
    }


};

class ProgressBar : public Widget
{
private:
    uint8_t status;
public:
    void set_Status(uint8_t status){
        if(status != this->status){
            this->dirty = true;
            this->status = status;
        }
    }
};


