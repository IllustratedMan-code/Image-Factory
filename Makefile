opencv:
	g++ image-factory.cpp -o image-factory `pkg-config --cflags --libs opencv4`
