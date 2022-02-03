#ifndef __COLOR_H__
#define __COLOR_H__

#include <stdint.h>
#include <string>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct Color {
    union {
        uint32_t c;
        struct {
            uint8_t a;
            uint8_t b;
            uint8_t g;
            uint8_t r;
        };
    };

    Color();
    // Color always consider the 4 channels RGBA in that order
    Color(uint32_t color);
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    Color(float r, float g, float b, float a = 1.f);
};

// Utility function
Color Hex2RGB(const std::string& hex);
Color Hex2RGBA(const std::string& hex);

glm::vec3 Color2Vec3(const Color& color);
glm::vec4 Color2Vec4(const Color& color);

void DebugColorRGB(const std::string& label, Color& color, bool compact = true);
void DebugColorRGBA(const std::string& label, Color& color, bool compact = true);

namespace ColorNames {
    // Got them from: https://htmlcolorcodes.com/color-names/
    
    //+ Red
    static const Color indianRed   {0xcd5c5c};
    static const Color lightCoral  {0xf08080};
    static const Color salmon      {0xfa8072};
    static const Color crimson     {0xdc143c};
    static const Color red         {0xff0000};
    static const Color fireBrick   {0xb22222};
    static const Color darkRed     {0x8b0000};

    //+ Pink
    static const Color pink            {0xffc0cb};
    static const Color lightPink       {0xffb6c1};
    static const Color hotPink         {0xff69b4};
    static const Color deepPink        {0xff1493};
    static const Color mediumVioletRed {0xc71585};
    static const Color paleVioletRed   {0xdb7093};

    //+ Orange
    static const Color lightSalmon {0xffa07a};
    static const Color darkSalmon  {0xe9967a};
    static const Color coral       {0xff7f50};
    static const Color tomato      {0xff6347};
    static const Color orangeRed   {0xff4500};
    static const Color darkOrange  {0xff8c00};
    static const Color orange      {0xffa500};

        //+ Yellow
    static const Color gold        {0xffd700};
    static const Color yellow      {0xffff00};
    static const Color lightYellow {0xffffe0};
    static const Color moccasin    {0xffe4b5};
    static const Color khaki       {0xf0e68c};
    static const Color darkKhaki   {0xbdb76b};

    //+ Purple
    static const Color lavender      {0xe6e6fa};;
    static const Color violet        {0xee82ee};
    static const Color orchid        {0xda70d6};
    static const Color fuchsia       {0xff00ff};
    static const Color magenta       {0xff00ff};
    static const Color mediumOrchid  {0xba55d3};
    static const Color mediumPurple  {0x9370db};
    static const Color blueViolet    {0x8a2be2};
    static const Color darkViolet    {0x9400d3};
    static const Color darkOrchid    {0x9932cc};
    static const Color darkMagenta   {0x8b008b};
    static const Color purple        {0x800080};
    static const Color indigo        {0x4b0082};
    static const Color slateBlue     {0x6a5acd};
    static const Color darkSlateBlue {0x483d8b};

    //+ Green
    static const Color greenYellow       {0xadff2f};
    static const Color lawnGreen         {0x7cfc00};
    static const Color lime              {0x00ff00};
    static const Color limeGreen         {0x32cd32};
    static const Color paleGreen         {0x98fb98};
    static const Color lightGreen        {0x90ee90};
    static const Color mediumSpringGreen {0x00fa9a};
    static const Color mediumSeaGreen    {0x3cb371};
    static const Color seaGreen          {0x2e8b57};
    static const Color green             {0x008000};
    static const Color darkGreen         {0x006400};
    static const Color yellowGreen       {0x9acd32};
    static const Color mediumAquamarine  {0x66cdaa};
    static const Color darkSeaGreen      {0x8fbc8b};
    static const Color lightSeaGreen     {0x20b2aa};
    static const Color darkCyan          {0x008b8b};
    static const Color teal              {0x008080};

    //+ Blue
    static const Color aqua            {0x00ffff};
    static const Color cyan            {0x00ffff};
    static const Color lightCyan       {0xe0ffff};
    static const Color paleTurquoise   {0xafeeee};
    static const Color aquamarine      {0x7fffd4};
    static const Color turquoise       {0x40e0d0};
    static const Color mediumTurquoise {0x48d1cc};
    static const Color darkTurquoise   {0x00ced1};
    static const Color steelBlue       {0x4682b4};
    static const Color lightSteelBlue  {0xb0c4de};
    static const Color lightBlue       {0xadd8e6};
    static const Color skyBlue         {0x87ceeb};
    static const Color lightSkyBlue    {0x87cefa};
    static const Color deepSkyBlue     {0x00bfff};;
    static const Color cornflowerBlue  {0x6495ed};
    static const Color mediumSlateBlue {0x7b68ee};
    static const Color royalBlue       {0x4169e1};
    static const Color blue            {0x0000ff};
    static const Color mediumBlue      {0x0000cd};
    static const Color darkBlue        {0x00008b};
    static const Color navy            {0x000080};
    static const Color midnightBlue    {0x191970};

    //+ Brown
    static const Color goldenrod      {0xdaa520};
    static const Color darkGoldenrod  {0xb8860b};
    static const Color peru           {0xcd853f};
    static const Color chocolate      {0xd2691e};
    static const Color sienna         {0xa0522d};
    static const Color brown          {0xa52a2a};
    static const Color maroon         {0x800000};

    //+ White
    static const Color white         {0xffffff};
    static const Color snow          {0xfffafa};
    static const Color azure         {0xf0ffff};
    static const Color aliceBlue     {0xf0f8ff};
    static const Color ghostWhite    {0xf8f8ff};
    static const Color whiteSmoke    {0xf5f5f5};
    static const Color seaShell      {0xfff5ee};
    static const Color beige         {0xf5f5dc};
    static const Color lavenderBlush {0xfff0f5};

    //+ Gray
    static const Color lightGray      {0xd3d3d3};
    static const Color silver         {0xc0c0c0};
    static const Color darkGray       {0xa9a9a9};
    static const Color gray           {0x808080};
    static const Color dimGray        {0x696969};
    static const Color lightSlateGray {0x778899};
    static const Color slateGray      {0x708090};
    static const Color darkSlateGray  {0x2f4f4f};
    static const Color black          {0x000000};

};

#endif // __COLOR_H__