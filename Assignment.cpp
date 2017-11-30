#include <TL-Engine.h>

using namespace tle;

void main() {
	I3DEngine* myEngine = New3DEngine(kTLX);
	myEngine->StartWindowed();

	myEngine->AddMediaFolder("./res");

	while(myEngine->IsRunning()) {
		myEngine->DrawScene();
	}

	myEngine->Delete();
}
