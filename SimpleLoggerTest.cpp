#include <iostream>
#include <sstream>
#include <fstream>

#include <string>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <map>
#include <queue>

#include "SimpleLogger.h"

using namespace s_logger;
using namespace std;


    


std::string &utf8to_translit(const std::string &in, std::string &out) {
    static const char *symbols[][2]  = {
        {"\xc3\x91", "N"}, /* Ñ */  {"\xc3\xb1", "n"}, /* ñ */ {"\xc3\x87", "C"}, /* Ç */ {"\xc3\xa7", "c"}, /* ç */ {"\xe2\x80\x94", ""}, /* MDASH */ 
        {"\xe2\x80\x93", "-"}, /* – NDASH */ {"\xe2\x80\x9c", ""}, /* “ */ {"\xe2\x80\x9d", ""}, /* ” */ {"\xc2\xab", ""}, /* « */ {"\xc2\xbb", ""}, /* » */
        {"\xe2\x80\x9e", ""}, /* „ */ {"\xe2\x80\x99", ""}, /* ’ */ {"\xe2\x80\x98", ""}, /* ‘ */ {"\xe2\x80\xb9", ""}, /* ‹ */ {"\xe2\x80\xba", ""}, /* › */
        {"\xe2\x80\x9a", ""}, /* ‚ */ {"\xc3\x9a", "U"}, /* Ú */ {"\xc3\x99", "U"}, /* Ù */ {"\xc3\x9b", "U"}, /* Û */ {"\xc3\x9c", "U"}, /* Ü */
        {"\xc3\xba", "u"}, /* ú */ {"\xc3\xb9", "u"}, /* ù */ {"\xc3\xbb", "u"}, /* û */ {"\xc3\xbc", "u"}, /* ü */ {"\xc3\x93", "O"}, /* Ó */
        {"\xc3\x92", "O"}, /* Ò */ {"\xc3\x94", "O"}, /* Ô */ {"\xc3\x95", "O"}, /* Õ */ {"\xc3\x96", "O"}, /* Ö */ {"\xc3\xb3", "o"}, /* ó */
        {"\xc3\xb2", "o"}, /* ò */ {"\xc3\xb4", "o"}, /* ô */ {"\xc3\xb5", "o"}, /* õ */ {"\xc2\xba", "o"}, /* º */ {"\xc3\xb6", "o"}, /* ö */
        {"\xc3\x89", "E"}, /* É */ {"\xc3\x88", "E"}, /* È */ {"\xc3\x8a", "E"}, /* Ê */ {"\xc3\x8b", "E"}, /* Ë */ {"\xc3\xa9", "e"}, /* é */
        {"\xc3\xa8", "e"}, /* è */ {"\xc3\xaa", "e"}, /* ê */ {"\xc3\xab", "e"}, /* ë */ {"\xc3\xad", "i"}, /* í */ {"\xc3\xac", "i"}, /* ì */
        {"\xc3\xae", "i"}, /* î */ {"\xc3\xaf", "i"}, /* ï */ {"\xc3\x8d", "I"}, /* Í */ {"\xc3\x8c", "I"}, /* Ì */ {"\xc3\x8e", "I"}, /* Î */
        {"\xc3\x8f", "I"}, /* Ï */ {"\xc3\x81", "A"}, /* Á */ {"\xc3\x80", "A"}, /* À */ {"\xc3\x82", "A"}, /* Â */ {"\xc3\x83", "A"}, /* Ã */
        {"\xc3\x84", "A"}, /* Ä */ {"\xc3\xa1", "a"}, /* á */ {"\xc3\xa0", "a"}, /* à */ {"\xc3\xa2", "a"}, /* â */ {"\xc3\xa3", "a"}, /* ã */
        {"\xc2\xaa", "a"}, /* ª */ {"\xc3\xa4", "a"}, /* ä */ {"\xd0\x99","Y"}, /* Й */ {"\xd0\xb9","y"}, /* й */ {"\xd0\xa6","Ts"}, /* Ц */
        {"\xd1\x86","ts"}, /* ц */ {"\xd0\xa3","U"}, /* У */ {"\xd1\x83","u"}, /* у */ {"\xd0\x9a","K"}, /* К */ {"\xd0\xba","k"}, /* к */
        {"\xd0\x95","E"}, /* Е */ {"\xd0\xb5","e"}, /* е */ {"\xd0\x9d","N"}, /* Н */ {"\xd0\xbd","n"}, /* н */ {"\xd0\x93","G"}, /* Г */
        {"\xd0\xb3","g"}, /* г */ {"\xd0\xa8","Sh"}, /* Ш */ {"\xd1\x88","sh"}, /* ш */ {"\xd0\xa9","Shch"}, /* Щ */ {"\xd1\x89","shch"}, /* щ */
        {"\xd0\x97","Z"}, /* З */ {"\xd0\xb7","z"}, /* з */ {"\xd0\xa5","Kh"}, /* Х */ {"\xd1\x85","kh"}, /* х */ {"\xd0\xaa",""}, /* Ъ */
        {"\xd1\x8a",""}, /* ъ */ {"\xd0\x81","E"}, /* Ё */ {"\xd1\x91","e"}, /* ё */ {"\xd0\xa4","F"}, /* Ф */ {"\xd1\x84","f"}, /* ф */
        {"\xd0\xab","Y"}, /* Ы */ {"\xd1\x8b","y"}, /* ы */ {"\xd0\x92","V"}, /* В */ {"\xd0\xb2","v"}, /* в */ {"\xd0\x90","A"}, /* А */ 
        {"\xd0\xb0","a"}, /* а */ {"\xd0\x9f","P"}, /* П */ {"\xd0\xbf","p"}, /* п */ {"\xd0\xa0","R"}, /* Р */ {"\xd1\x80","r"}, /* р */
        {"\xd0\x9e","O"}, /* О */ {"\xd0\xbe","o"}, /* о */ {"\xd0\x9b","L"}, /* Л */ {"\xd0\xbb","l"}, /* л */ {"\xd0\x94","D"}, /* Д */
        {"\xd0\xb4","d"}, /* д */ {"\xd0\x96","Zh"}, /* Ж */ {"\xd0\xb6","zh"}, /* ж */ {"\xd0\xad","E"}, /* Э */ {"\xd1\x8d","e"}, /* э */
        {"\xd0\xaf","Ya"}, /* Я */ {"\xd1\x8f","ya"}, /* я */ {"\xd0\xa7","Ch"}, /* Ч */ {"\xd1\x87","ch"}, /* ч */ {"\xd0\xa1","S"}, /* С */
        {"\xd1\x81","s"}, /* с */ {"\xd0\x9c","M"}, /* М */ {"\xd0\xbc","m"}, /* м */ {"\xd0\x98","I"}, /* И */ {"\xd0\xb8","i"}, /* и */
        {"\xd0\xa2","T"}, /* Т */ {"\xd1\x82","t"}, /* т */ {"\xd0\xac",""}, /* Ь */ {"\xd1\x8c",""}, /* ь */ {"\xd0\x91","B"}, /* Б */
        {"\xd0\xb1","b"}, /* б */ {"\xd0\xae","Yu"}, /* Ю */ {"\xd1\x8e","yu"} /* ю */
    };
    static const size_t symb_size = sizeof(symbols) / sizeof(symbols[0]);
    out = in;
    for (int i = 0; i < symb_size; ++i) {
        size_t n = std::string::npos;
        while ((n = out.find(symbols[i][0])) != std::string::npos) {
            out.replace(n, strlen(symbols[i][0]), symbols[i][1]);
        }
    }

    string out_r; out_r.reserve(out.size());
    
    for (const char &c : out) {
        if (c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c == '_' || c == '-') {
            out_r += c;
        }
    }
    out_r.swap(out);
    return out;
}

void f1() {
    for (int i = 0; i < 10; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        LOG(TRACE) << i;
        LOGF(TRACE, "%i", i);
    }
}


int main()
{
    create_logger("def", true, "test.%datetime.log");
    /*
    std::thread t2(f1);
    
    LOG(DEBUG) << "фывфывалорфывлоар";
    
    LOG(DEBUG) << "Test Test Test";
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    LOG(FATAL) << "Test2 Test2 Test2";

    for (int i = 0; i < 10; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        LOG(TRACE) << i;
    }
    t2.join();
    */
    string a;
    CLOG("def",DEBUG) << "";
    LOG(DEBUG) << "";
    LOG(DEBUG) << utf8to_translit(u8"asdfasdыапавпывапавдлоñ–", a);
    LOG(DEBUG) << "";
    {
        const log_writer& w = CLOG("def", DEBUG);

    }

}

