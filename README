<img src="ValhallaEngine/assets/Assets.xcassets/AppIcon.appiconset/ValhallaEngineIcon_512x512.png" width=200>

# Valhalla Engine 

An open-source 3D Game Engine 

## Quick Information
### Dependencies
- fmt : For quick logging
- Vulkan (Windows / Linux) : For the graphics API
- Metal (Mac) : For the graphics API 
- DirectX 12 (Xbox/Windows) : For the graphics API **(Optional)**
- Windows SDK : Required to have a functional game output
- MacOS SDK : Required to have a functional engine and game output
- Qt6 (Editor UI)

### Why Does This Exist?
This engine is a side project I do to prove anyone can build a game engine far better than **Unreal Engine 5.**

### How can I contribute?
You can submit a pull request or email me. I'm sure to get back to you eventually.

### Why use the MIT license instead of GPLv2? 
I don't personally care if you use code in my engine without giving back. I'm not like **Linus Torvalds** (even through I respect the guy) this is meant to be a <u>fun hobby or project not a controlled codespace.</u> There are also coding standards. This is just for readability and constancy

## Compiling
### How do I build for MacOS?
First you need to download the Qt6 SDK for MacOS along with Xcode and run the &rarr; compilemac.sh shell script. 

*Also, if you wanted to have your Mac version to use Vulkan instead of Metal (for what reason I don't know) you will have to install the Vulkan SDK*
```
./compilemac.sh
```
If you get a permission error when trying to run the script then do the following 
```
chmod +x compilemac.sh
``` 


# Coding standards
### Brackets

Whatever you do don't put your brackets like this. This just becomes confusing when there are lines of code and a bunch of **if** or **else** statements. I won't accept code that looks like this. If you send me code please don't write like this or just expect me to reject based on standards grounds
```cpp
int function(uint32_t a, uint32_t b) {
    if (a > b) {
        return 0;
    }
    else if (a < b) {
        return 1;
    }
    else {
        return 2;
    }
}
```

Write your brackets like this! 

```cpp
int function(uint32_t a, uint32_t b) 
{
    if (a > b) 
    {
        return 0;
    }
    else if (a < b) 
    {
        return 1;
    }
    else 
    {
        return 2;
    }
}
```

### Pointers and references

I didn't think people actually wrote pointers and references this way but just don't. No questions asked

```cpp
bool function(const char *a, const string &b)
{
    return true;
}
```

Write them as god intended and also as the word pointer describes &rarr; var

```cpp
bool function(const char* a, const string& b)
{
    return true;
}
```

### Enumerations

This is a personally habit of mine but never have your enums as this

```cpp
enum JunkyFixType
{
    crack,
    cocaine,
    heroin,
    meth,
    speed,
    dmt,
    weed
};

// same applies if you use enum class or not
```

Write them as this but always start with undefined before anything else

```cpp
enum JunkyFixType
{
    JFT_UNDEFINED,
    JFT_CRACK,
    JFT_COCAINE,
    JFT_HEROIN,
    JFT_METH,
    JFT_SPEED,
    JFT_DMT,
    JFT_WEED
};

// same applies if you use enum class or not
```

### Naming

I personally name my things like this but don't do them like the python way. 

```cpp
class my_class
{
public: 
    const char* my_string;

    bool my_function(bool my_statement);
};
```

Instead do it like this

```cpp
class MyClass
{
public:
    const char* MyString;

    bool MyFunction(bool MyStatement);
};
```

### Macros functions

I don't have an issue with macro functions but never make it look like this mess

```cpp
#define Macro(a) \
    if (a == 0xFF) \
    { \ 
        printf("Max 8-bit value"); \
    } 
```

Make it look nice like this 

```cpp
#define Macro(a)                    \
    if (a == 0xFF)                  \
    {                               \
        printf("Max 8-bit value");  \
    }
```

### Classes or structures

Never put a space between **public**, **protected**, and **private** inside the **class** or **struct** like this

```cpp
class JarOfDirt
{
    public:
        uint64_t GetDirtAmount() { return DirtAmount; } /*
    I don't have an issue if you do this with a short variable and 
    function name.            
*/

    private:
        uint64_t DirtAmount;
};

struct DavyJones
{    
    protected:
        const char* HisHeart;
};
```

Please do it like this

```cpp
class JarOfDirt
{
public:
    uint64_t GetDirtAmount() const { return DirtAmount; } /*
    I don't have an issue if you do this with a short variable and 
    function name.            
*/

private:
    uint64_t DirtAmount;
};

struct DavyJones
{    
protected:
    const char* HisHeart;
};
```

Also if you have a function that accesses a **protected** or **private** variable here's the guidelines for that 

```cpp
class BlackBeard
{
public:
    uint32_t GetTears() const { return MermaidTears; } // Approved!

    uint16_t RetrieveThisVariable() const { return ASuperLongVariableThatFillsUpSpace; } // Not allowed!

    // For a long function and variable name do this!
    uint16_t GetThisVariable() const 
    {
        return ASuperLongVariableThatFillsUpSpace; // Approved!
    }
private:
    uint32_t MermaidTears;

    uint16_t ASuperLongVariableThatFillsUpSpace;
};
```