#ifndef GENERICCONFIG_HPP
# define GENERICCONFIG_HPP

class GenericConfig
{
    protected:
        void extractKey(std::string &key, std::size_t &colonPos);
        void extractValue(std::string &value, std::size_t &colonPos);
        void errorExit(std::string err1, std::string err2);
        bool countTabIndents(int numTabs);
        void detectAndStripTabIndents(int numTabs);
}

#endif