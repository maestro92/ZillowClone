Things I learned

1. nested function in C++

criteria
 - It is called in more than once place. (otherwise just write the code directly if it's not too long)
 - It is really an internal function, so that that calling it in any other context would not make sense.
 - It's short enough (maybe 10 lines max).

lifetime
because it's an address of a function, and functions have no lifetime, you are free to call that function whenever you'd like. Everything you have is well-defined.
http://stackoverflow.com/questions/8026170/lifetime-of-lambda-objects-in-relation-to-function-pointer-conversion

int main() 
{
    auto f = []() { return 42; };
    std::cout << "f() = " << f() << std::endl;
}
http://stackoverflow.com/questions/4324763/c-can-we-have-functions-inside-functions



2. Function pointer (for my button callBacks)
http://www.tutok.sk/fastgl/callback.html

std::function and std::bind
http://oopscenities.net/2012/02/24/c11-stdfunction-and-stdbind/
http://stackoverflow.com/questions/14189440/c-class-member-callback-simple-examples




3. UV coordinates
left top		right top
	(0,1)		(1,1)	


	(0,0)		(1,0)
left bottom 	right bottom


4. All About Rendering Text

font size vs pixel size

http://graphicdesign.stackexchange.com/questions/4035/what-does-the-size-of-the-font-translate-to-exactly
http://learnopengl.com/#!In-Practice/Text-Rendering


instanced rendering?

word wrap/line break algorithm



5. In rendering GUI, we often want to set the top left as (0,0) so y increases as it the screen coordinates goes
down. the trick is to
http://stackoverflow.com/questions/25416820/set-origin-to-top-left-corner-of-screen-in-opengl-es-2

invert the bottom with top in the ortho matrix