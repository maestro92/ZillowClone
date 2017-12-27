#include "utility.h"


void Utility::debugLn(int l)
{
    for(int i=0; i<l; i++)
        cout << endl;
}

void Utility::debugLn(string s, int l)
{
    debug(s);
    debugLn(l);
}

void Utility::debug(string s)
{
    cout << s << endl;
}

void Utility::debug(string s, bool b)
{
    if(b)
        cout << "true" << endl;
    else
        cout << "false" << endl;
}

void Utility::debug(string s, char c)
{
    cout << s << "  "  << c << endl;
}

void Utility::debug(string s, string s2)
{
    cout << s << "  "  << s2 << endl;
}

void Utility::debug(string s, unsigned int i)
{
    cout << s << "  " << i << endl;
}

void Utility::debug(string s, int i)
{
    cout << s << " " << i << endl;
}

void Utility::debug(string s, float f)
{
    cout << s << " " << f << endl;
}

void Utility::debug(string s, glm::vec2 v)
{
    cout << s << " " << v.x << " " << v.y << endl << endl;
}

void Utility::debug(string s, glm::vec3 v)
{
    cout << s << "  " << v.x << " " << v.y << " " << v.z << endl;
}

void Utility::debug(string s, glm::vec4 v)
{
    cout << s << "  " << v.x << " " << v.y << " " << v.z << " " << v.w << endl;
}

void Utility::debug(string s, glm::mat3 m)
{
    cout << s << endl;
    for (int i=0; i<3; i++)
        cout << m[i][0] << " " << m[i][1] << " " << m[i][2] << endl;

    cout << endl << endl;
}

void Utility::debug(string s, glm::mat4 m)
{
    cout << s << endl;
    for (int i=0; i<4; i++)
        cout << m[i][0] << " " << m[i][1] << " " << m[i][2] << " " << m[i][3] << endl;

    cout << endl << endl;
}
