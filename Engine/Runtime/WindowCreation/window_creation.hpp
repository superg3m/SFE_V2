
struct Window {
	void* ctx;
	static void create();
	bool should_close();
	void close();
};