#include <lo/lo.h>
#include <lo/lo_cpp.h>

#include "RepClassifier.h"
//#include "RepClassifier.cpp"

#include <iostream>
#include <atomic>
#include <unistd.h>
#include <string.h>
#include <vector>

using namespace std;

// Parameter
double scale = 100.; //Q: why do we need the scale?

// State variables
bool busy = false;
int wasLearning = -1;
bool wasSync = false;
bool wasDetected = false;

// Objects
RepClassifier* rc;
lo::Address* a;
lo::ServerThread *st;

// lo::Server *st;


// bool shouldQuit = false;
std::atomic<bool> shouldQuit(false);

void data_handler(const char *path, const char *types, lo_arg **argv, int argc)
{
  //TODO: put all data in one bundle -- but: no way to send bundles from serverthread in cpp version of liblo...
  //TODO: send messages from server thread -- no way to send bundles from serverthread?
    lo::Bundle oscbundle;
    std::vector<float> vec;
    vec.push_back(argv[0]->f * scale);
    vec.push_back(argv[1]->f * scale);
    vec.push_back(argv[2]->f * scale);

    busy = true;
    rc->infer(vec);
    int g = rc->mostLikelyGesture();
    if (g>=0)
    {
        if(!wasDetected)
        {
            wasDetected = true;
	    oscbundle.add( "/OSCGestureClassifier/detected", lo::Message( "i", 1 ) );
//             a->send_from( st, "/OSCGestureClassifier/detected","i",1);
// 	    a->send( "/OSCGestureClassifier/detected","i",1);
        }
        oscbundle.add( "/OSCGestureClassifier/index", lo::Message( "i", rc->mostLikelyGesture() ) );
	oscbundle.add( "/OSCGestureClassifier/distance", lo::Message( "f", rc->getDistance() ) );
	oscbundle.add( "/OSCGestureClassifier/phase", lo::Message( "f", rc->getPhase() ) );
	oscbundle.add( "/OSCGestureClassifier/detected/all", lo::Message( "iiiff", 1, rc->isSync(), rc->mostLikelyGesture(), rc->getDistance(), rc->getPhase() ) );
	
//         a->send_from( st, "/OSCGestureClassifier/index","i",rc->mostLikelyGesture());
// 	a->send_from( st, "/OSCGestureClassifier/distance","f",rc->getDistance());
// 	a->send_from( st, "/OSCGestureClassifier/phase","f",rc->getPhase());
//         a->send( lo::Bundle(
//         {
//             {"/OSCGestureClassifier/index",lo::Message("i",rc->mostLikelyGesture())},
//             {"/OSCGestureClassifier/distance", lo::Message("f",rc->getDistance())},
//             {"/OSCGestureClassifier/phase", lo::Message("f",rc->getPhase())}
//         } ));
    }
    else if(wasDetected)
    {
        wasDetected = false;
	oscbundle.add( "/OSCGestureClassifier/detected", lo::Message( "i", 0 ) );
//         a->send_from(st, "/OSCGestureClassifier/detected","i",0);
// 	a->send("/OSCGestureClassifier/detected","i",0);
    }


    if(wasLearning >= 0 && !rc->isLearning())
    {
        std::cout << "Gesture learned! Length is: " << rc->templateSize(wasLearning) << "\n";
//         a->send_from( st, "/OSCGestureClassifier/learned", "i", wasLearning);
// 	a->send( "/OSCGestureClassifier/learned", "i", wasLearning);
	oscbundle.add( "/OSCGestureClassifier/learned", lo::Message( "i", wasLearning ) );
	oscbundle.add( "/OSCGestureClassifier/learning", lo::Message( "iii", rc->isLearning(), rc->isRecording(), rc->templateSize(wasLearning) ) );
        wasLearning = -1;
//        rc->learn();
    } else if ( rc->isLearning() ){
// 	a->send_from( st, "/OSCGestureClassifier/learning", "ii", rc->isLearning(), rc->isRecording() );
// 	a->send( "/OSCGestureClassifier/learning", "ii", rc->isLearning(), rc->isRecording() );
	oscbundle.add( "/OSCGestureClassifier/learning", lo::Message( "iii", rc->isLearning(), rc->isRecording(), rc->templateSize(wasLearning) ) );
    }

    if(rc->isSync())
    {
        if(!wasSync)
        {
            wasSync = true;
//            a->send_from( st, "/OSCGestureClassifier/repetition", "i", 1);
// 	      a->send( "/OSCGestureClassifier/repetition", "i", 1);
	      oscbundle.add( "/OSCGestureClassifier/repetition", lo::Message( "i", 1 ) );	
        }
//         a->send( "/OSCGestureClassifier/interval","i",rc->repetitionInterval());
	oscbundle.add( "/OSCGestureClassifier/interval", lo::Message( "i", rc->repetitionInterval() ) );	
	oscbundle.add( "/OSCGestureClassifier/repetition/all", lo::Message( "ii", 1, rc->repetitionInterval() ) );	
//         a->send_from( st, "/OSCGestureClassifier/interval","i",rc->repetitionInterval());
    }
    else if(wasSync)
    {
        wasSync = false;
//         a->send_from( st, "/OSCGestureClassifier/repetition", "i", 0);
// 	a->send( "/OSCGestureClassifier/repetition", "i", 0);
	oscbundle.add( "/OSCGestureClassifier/repetition", lo::Message( "i", 0 ) );	
    }

    oscbundle.add( "/OSCGestureClassifier/update", lo::Message( "iii", rc->isSync(), wasDetected, rc->isLearning() ) );	
//     a->send( "/OSCGestureClassifier/update", "iii",  rc->isSync(), wasDetected, rc->isLearning());
//     a->send_from( st, oscbundle );
    a->send( oscbundle );
    busy = false;
}

void learn_handler(const char *path, const char *types, lo_arg **argv, int argc)
{
    busy = true;
    rc->learn();
    wasLearning = rc->size()-1;
    cout << "Learning gesture " << rc->size() << "\n";
    busy = false;
}

void relearn_handler(const char *path, const char *types, lo_arg **argv, int argc)
{
    busy = true;
    rc->learn( (argv[0]->i) );
    wasLearning = (argv[0]->i);
    cout << "Relearning gesture " << (argv[0]->i) << "\n";
    busy = false;
}

void learngate_handler(const char *path, const char *types, lo_arg **argv, int argc)
{
    busy = true;
    rc->setLearningGate( (bool)(argv[0]->i) );
    cout << "Learning gate " << argv[0]->i << "\n";
    busy = false;
}

void recognize_handler(const char *path, const char *types, lo_arg **argv, int argc)
{
    busy = true;
    rc->stopLearning();
    cout << "Stopped learning\n";
    busy = false;
}

void clear_handler(const char *path, const char *types, lo_arg **argv, int argc)
{
    busy = true;
    rc->clear();
    wasLearning = -1;
    cout << "Classifier cleared\n";
    busy = false;
}

void minimum_template_size_handler(const char *path, const char *types, lo_arg **argv, int argc)
{
    busy = true;
    rc->setMinimumTemplateSize((int)argv[0]->i);
    cout << "minimum template size set to: " << argv[0]->i << "\n";
    busy = false;
}

void threshold_handler(const char *path, const char *types, lo_arg **argv, int argc)
{
    busy = true;
    rc->setRecognitionThreshold((double)argv[0]->f);
    cout << "threshold set to: " << argv[0]->f << "\n";
    busy = false;
}

void quit_handler(const char *path, const char *types, lo_arg **argv, int argc)
{
    busy = true;
    shouldQuit = true;
    cout << "Received quit from osc " << "\n";
    busy = false;
}

void yin_average_threshold_handler(const char *path, const char *types, lo_arg **argv, int argc)
{
    busy = true;
    rc->setAverageThreshold( argv[0]->f );
    cout << "YIN: set average threshold " << argv[0]->f << "\n";
    busy = false;
}

void yin_dip_threshold_handler(const char *path, const char *types, lo_arg **argv, int argc)
{
    busy = true;
    rc->setDipThreshold( argv[0]->f );
    cout << "YIN: set dip threshold " << argv[0]->f << "\n";
    busy = false;
}

void yin_max_length_handler(const char *path, const char *types, lo_arg **argv, int argc)
{
    busy = true;
    rc->setMaxLength( argv[0]->i );
    cout << "YIN: set max length " << argv[0]->i << "\n";
    busy = false;
}

void yin_max_delay_handler(const char *path, const char *types, lo_arg **argv, int argc)
{
    busy = true;
    rc->setMaxDelay( argv[0]->i );
    cout << "YIN: set max delay " << argv[0]->i << "\n";
    busy = false;
}

void yin_min_dips_handler(const char *path, const char *types, lo_arg **argv, int argc)
{
    busy = true;
    rc->setMinDips( argv[0]->i );
    busy = false;
}

void default_handler(const char *path, const char *types, lo_arg **argv, int argc)
{
    std::cout << "Sorry, but " << path << " " << types << " is not a valid command message...\n";
}

void help()
{
    std::cout << "+==============================================================================\n";
    std::cout << "+====================          OSCGestureClassifier          ==================\n";
    std::cout << "+==============================================================================\n";
    std::cout << "\n";
    std::cout << "Send messages:\n";
    std::cout << "/data f f f \t3-dimensional data such as accelerometer or Kinect data\n";
    std::cout << "/learn \t\tto start recording a new template on repeated movement\n";
    std::cout << "/learn/gate i\t\tto enable recording even when no sync is there\n";
    std::cout << "/clear \t\tto clear the classifier\n";
    std::cout << "/theshold f \tto set the recognition threshold\n";
    std::cout << "\n";
    std::cout << "When learning, the algorithm will still try to recognize the gesures.";
    std::cout << "If the distance between the performed gesture and a template is below \n";
    std::cout << "the threshold, the gesture is recognized. If that distance is above the \n";
    std::cout << "threshold, a new gesture is learned.\n";
    std::cout << "\n";
    std::cout << "Messages back:\n";
    std::cout << "/OSCGestureClassifier/repetition i \t1 or 0 whether repetition is detected\n";
    std::cout << "/OSCGestureClassifier/interval i \tlength of the detected repetition \n";
    std::cout << "/OSCGestureClassifier/detected i \t1 or 0 whether template is detected \n";
    std::cout << "/OSCGestureClassifier/index i \t\tindex of detected template\n";
    std::cout << "/OSCGestureClassifier/distance f \tdistace from gesture to detected template\n";
    std::cout << "/OSCGestureClassifier/phase f \t\tcurrent phase of performed gesture\n";
    std::cout << "/OSCGestureClassifier/learned i \tsize of vocabulary\n";
    std::cout << "\n";
    std::cout << "+==============================================================================\n";
}

int main(int argc, char** argv)
{
    char *myport = "9005";
    char *dest_port = "9006";
    char *dest_ip = "127.0.0.1";

    if ( argc > 3 )
        dest_ip = argv[3];
    if ( argc > 2 )
        myport = argv[2];
    if ( argc > 1 )
        dest_port = argv[1];


//     lo::ServerThread 
    st = new lo::ServerThread(myport);
//     st = new lo::Server(myport);
    if (!st->is_valid())
    {
        std::cout << "Nope." << std::endl;
        return 1;
    }

    a = new lo::Address("127.0.0.1", dest_port);

    std::cout << "Comming in at: " << st->url() << std::endl;
    std::cout << "Back at you at: " << a->url() << std::endl << std::endl;
    std::cout << "Press h for Help\n";
    std::cout << "Want to quit? Hit 'q' and Enter\n\n";

//     std::atomic<int> received(0);

    rc = new RepClassifier();
    rc->setResolution(6);

    st->add_method("/data","fff",data_handler);
    st->add_method("/learn","",learn_handler);
    st->add_method("/learn/again","i",relearn_handler);
    st->add_method("/learn/gate","i",learngate_handler);
    st->add_method("/recognize","",recognize_handler);
    st->add_method("/threshold","f",threshold_handler);
    st->add_method("/minimum_template_size","i",minimum_template_size_handler);
    st->add_method("/clear","",clear_handler);
    st->add_method("/quit","",quit_handler);
    
    st->add_method("/yin/average_threshold","f",yin_average_threshold_handler);
    st->add_method("/yin/dip_threshold","f",yin_dip_threshold_handler);
    st->add_method("/yin/max_length","i",yin_max_length_handler);
    st->add_method("/yin/max_delay","i",yin_max_delay_handler);
    st->add_method("/yin/min_dips","i",yin_min_dips_handler);
    
    st->add_method(NULL,NULL,default_handler);

    st->start();

    char s[10];
    
    shouldQuit = false;

    std::cout << "If you want to quit, enter 'q'\n";
    while ( !shouldQuit )
    {
      usleep(1000);
//       st->recv(100);
      std::cin.getline(s,10);
      if(strcmp(s,"q")==0)
	shouldQuit = true;
      else if(strcmp(s, "h")==0)
            help();
    }

    std::cout << "\nClosing time...\n";
}
