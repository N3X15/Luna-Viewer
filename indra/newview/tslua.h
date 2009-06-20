#ifndef TS_TSLUA_H
#define TS_TSLUA_H

class TSInternalLua;

class TSLua {
public:
	TSLua();
	virtual ~TSLua();
	virtual void output(const char *s);
	virtual void error(const char *s);
	int execute(const char *s);
	virtual bool abortCondition();

private:
	TSInternalLua *mInternalLua;
};

#endif