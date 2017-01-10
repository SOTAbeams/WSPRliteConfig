#ifndef common_RelOps_h
#define common_RelOps_h

// Like std::rel_ops, but using CRTP to avoid some problems caused by "using namespace std::rel_ops"

template<class T>
class RelOps
{
public:
	bool operator!=(const T &b) const
	{
		const T &a = *static_cast<const T*>(this);
		return !(a == b);
	}
	bool operator>(const T &b) const
	{
		const T &a = *static_cast<const T*>(this);
		return b < a;
	}
	bool operator<=(const T &b) const
	{
		const T &a = *static_cast<const T*>(this);
		return !(b < a);
	}
	bool operator>=(const T &b) const
	{
		const T &a = *static_cast<const T*>(this);
		return !(a < b);
	}
};

#endif
