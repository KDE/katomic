
class atom {
 public:
    char obj;
    unsigned char conn;

    bool operator==(const atom& rhs) const { return (rhs.obj == obj && rhs.conn == conn); }
    bool isEmpty() const { return (obj == 0 || obj == '.'); }
};

inline char int2atom(int i) { 
    if (!i)
	return '.';
    if (i <= 9) 
	return i + '0';
    return i + 'a' - 10;
}

inline int atom2int(char ch) {
    if (ch == '.' || ch == 0)
	return 0;
    if (ch >= '0' && ch <= '9')
	return ch - '0';
    return ch - 'a' + 10;
}
