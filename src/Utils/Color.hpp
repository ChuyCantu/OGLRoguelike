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
    static const Color indianRed   {0xcd5c5cff};
    static const Color lightCoral  {0xf08080ff};
    static const Color salmon      {0xfa8072ff};
    static const Color crimson     {0xdc143cff};
    static const Color red         {0xff0000ff};
    static const Color fireBrick   {0xb22222ff};
    static const Color darkRed     {0x8b0000ff};

    //+ Pink
    static const Color pink            {0xffc0cbff};
    static const Color lightPink       {0xffb6c1ff};
    static const Color hotPink         {0xff69b4ff};
    static const Color deepPink        {0xff1493ff};
    static const Color mediumVioletRed {0xc71585ff};
    static const Color paleVioletRed   {0xdb7093ff};

    //+ Orange
    static const Color lightSalmon {0xffa07aff};
    static const Color darkSalmon  {0xe9967aff};
    static const Color coral       {0xff7f50ff};
    static const Color tomato      {0xff6347ff};
    static const Color orangeRed   {0xff4500ff};
    static const Color darkOrange  {0xff8c00ff};
    static const Color orange      {0xffa500ff};

        //+ Yellow
    static const Color gold        {0xffd700ff};
    static const Color yellow      {0xffff00ff};
    static const Color lightYellow {0xffffe0ff};
    static const Color moccasin    {0xffe4b5ff};
    static const Color khaki       {0xf0e68cff};
    static const Color darkKhaki   {0xbdb76bff};

    //+ Purple
    static const Color lavender      {0xe6e6faff};;
    static const Color violet        {0xee82eeff};
    static const Color orchid        {0xda70d6ff};
    static const Color fuchsia       {0xff00ffff};
    static const Color magenta       {0xff00ffff};
    static const Color mediumOrchid  {0xba55d3ff};
    static const Color mediumPurple  {0x9370dbff};
    static const Color blueViolet    {0x8a2be2ff};
    static const Color darkViolet    {0x9400d3ff};
    static const Color darkOrchid    {0x9932ccff};
    static const Color darkMagenta   {0x8b008bff};
    static const Color purple        {0x800080ff};
    static const Color indigo        {0x4b0082ff};
    static const Color slateBlue     {0x6a5acdff};
    static const Color darkSlateBlue {0x483d8bff};

    //+ Green
    static const Color greenYellow       {0xadff2fff};
    static const Color lawnGreen         {0x7cfc00ff};
    static const Color lime              {0x00ff00ff};
    static const Color limeGreen         {0x32cd32ff};
    static const Color paleGreen         {0x98fb98ff};
    static const Color lightGreen        {0x90ee90ff};
    static const Color mediumSpringGreen {0x00fa9aff};
    static const Color mediumSeaGreen    {0x3cb371ff};
    static const Color seaGreen          {0x2e8b57ff};
    static const Color green             {0x008000ff};
    static const Color darkGreen         {0x006400ff};
    static const Color yellowGreen       {0x9acd32ff};
    static const Color mediumAquamarine  {0x66cdaaff};
    static const Color darkSeaGreen      {0x8fbc8bff};
    static const Color lightSeaGreen     {0x20b2aaff};
    static const Color darkCyan          {0x008b8bff};
    static const Color teal              {0x008080ff};

    //+ Blue
    static const Color aqua            {0x00ffffff};
    static const Color cyan            {0x00ffffff};
    static const Color lightCyan       {0xe0ffffff};
    static const Color paleTurquoise   {0xafeeeeff};
    static const Color aquamarine      {0x7fffd4ff};
    static const Color turquoise       {0x40e0d0ff};
    static const Color mediumTurquoise {0x48d1ccff};
    static const Color darkTurquoise   {0x00ced1ff};
    static const Color steelBlue       {0x4682b4ff};
    static const Color lightSteelBlue  {0xb0c4deff};
    static const Color lightBlue       {0xadd8e6ff};
    static const Color skyBlue         {0x87ceebff};
    static const Color lightSkyBlue    {0x87cefaff};
    static const Color deepSkyBlue     {0x00bfffff};;
    static const Color cornflowerBlue  {0x6495edff};
    static const Color mediumSlateBlue {0x7b68eeff};
    static const Color royalBlue       {0x4169e1ff};
    static const Color blue            {0x0000ffff};
    static const Color mediumBlue      {0x0000cdff};
    static const Color darkBlue        {0x00008bff};
    static const Color navy            {0x000080ff};
    static const Color midnightBlue    {0x191970ff};

    //+ Brown
    static const Color goldenrod      {0xdaa520ff};
    static const Color darkGoldenrod  {0xb8860bff};
    static const Color peru           {0xcd853fff};
    static const Color chocolate      {0xd2691eff};
    static const Color sienna         {0xa0522dff};
    static const Color brown          {0xa52a2aff};
    static const Color maroon         {0x800000ff};

    //+ White
    static const Color white         {0xffffffff};
    static const Color snow          {0xfffafaff};
    static const Color azure         {0xf0ffffff};
    static const Color aliceBlue     {0xf0f8ffff};
    static const Color ghostWhite    {0xf8f8ffff};
    static const Color whiteSmoke    {0xf5f5f5ff};
    static const Color seaShell      {0xfff5eeff};
    static const Color beige         {0xf5f5dcff};
    static const Color lavenderBlush {0xfff0f5ff};

    //+ Gray
    static const Color lightGray      {0xd3d3d3ff};
    static const Color silver         {0xc0c0c0ff};
    static const Color darkGray       {0xa9a9a9ff};
    static const Color gray           {0x808080ff};
    static const Color dimGray        {0x696969ff};
    static const Color lightSlateGray {0x778899ff};
    static const Color slateGray      {0x708090ff};
    static const Color darkSlateGray  {0x2f4f4fff};
    static const Color black          {0x000000ff};

};

#endif // __COLOR_H__