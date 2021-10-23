#include <string>

class Json {
public:
	Json(std::string* keys, int length);
	//update(std::string* values);
	//parse(std::string json);
	//get();
private:
	std::string* keys;
	std::string* values;
	int length;
};
