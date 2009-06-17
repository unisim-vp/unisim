template <class T>
class SymbolTableLookup
{
public:
	virtual const typename unisim::util::debug::Symbol<T> *FindSymbol(
		const char *name,
		T addr,
		typename unisim::util::debug::Symbol<T>::Type type) const = 0;

	virtual const typename unisim::util::debug::Symbol<T> *FindSymbolByAddr(T addr) const = 0;

	virtual const typename unisim::util::debug::Symbol<T> *FindSymbolByName(const char *name) const = 0;

	virtual const typename unisim::util::debug::Symbol<T> *FindSymbolByName(
		const char *name,
		typename unisim::util::debug::Symbol<T>::Type type) const = 0;

	virtual const typename unisim::util::debug::Symbol<T> *FindSymbolByAddr(
		T addr,
		typename unisim::util::debug::Symbol<T>::Type type) const = 0;
};
