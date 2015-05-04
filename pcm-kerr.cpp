#include "./PCM/cpucounters.h"
#define SIZE 1000
using namespace std;
int main(){
	float matrixa[SIZE][SIZE], matrixb[SIZE][SIZE], mresult[SIZE][SIZE];
	float real_time, proc_time, mflops;
	long long flpins;
	int retval;
	int i,j,k;

	PCM *m = PCM::getInstance();
	PCM::ErrorCode status = m->program();
	switch(status)
	{
	case PCM::Success:
	    break;
	case PCM::MSRAccessDenied:
	    cerr<< "Access to Intel(r) Performance Counter Monitor has denied (no MSR or PCI CFG space access).)" << endl;
	default:
	    exit(EXIT_FAILURE);
	}
	
	cerr<< "\nDetected " << m->getCPUBrandString() << " \"Intel(r) microarchitecture codename " << m->getUArchCodename() << "\"" << endl;

	SystemCounterState sstate1, sstate2; 
	std::vector<CoreCounterState> cstate1, cstate2;
	std::vector<SocketCounterState> sktstate1, sktstate2;

	m->getAllCounterStates(sstate1, sktstate1, cstate1);
	
	// Begin of custom code
	for ( i=0; i<SIZE*SIZE; i++ ){
    		mresult[0][i] = 0.0;
    		matrixa[0][i] = matrixb[0][i] = rand()*(float)1.1; 
	}
	for (i=0;i<SIZE;i++)
    		for(j=0;j<SIZE;j++)
      			for(k=0;k<SIZE;k++)
        			mresult[i][j]=mresult[i][j] + matrixa[i][k]*matrixb[k][j];

	m->getAllCounterStates(sstate2, sktstate2, cstate2);
	
	cout.precision(3);
	cout << std::fixed;

	cout << " Core (SKT) | EXEC | IPC | FREQ | L3HITS | L3HITR | L2HITS | L2HITR" << endl;
	for (uint32 i=0; i < m->getNumCores(); ++i)
	{
	    if (m->isCoreOnline(i) == false)
		continue;

	    cout << " " << setw(3) << i << "	" << setw(2) << m->getSocketId(i) <<
		"   " << setw(5) << getExecUsage(cstate1[i], cstate2[i]) <<
		"   " << setw(5) << getIPC(cstate1[i], cstate2[i]) <<
		"   " << setw(5) << getRelativeFrequency(cstate1[i], cstate2[i]) << 
		"   " << setw(5) << getL3CacheHits(cstate1[i], cstate2[i]) <<
		"   " << setw(5) << getL3CacheHitRatio(cstate1[i], cstate2[i]) <<
		"   " << setw(5) << getL2CacheHits(cstate1[i], cstate2[i]) << 
		"   " << setw(5) << getL2CacheHitRatio(cstate1[i], cstate2[i]) << endl;

	}
    
	cout << "--------------------------------------------------" << "\n";
	
	//we can find that, L3CacheHits by sstate == sum(L3CacheHits by cores)
	//But, total L2HitRatio is not the average of 4 cores, and should be weight mean

	cout << " TOTAL	*   " << setw(5) << getExecUsage(sstate1, sstate2) <<
	"   " << setw(5) << getIPC(sstate1, sstate2) <<
	"   " << setw(5) << getRelativeFrequency(sstate1, sstate2) <<
	"   " << setw(5) << getL3CacheHits(sstate1, sstate2) <<
	"   " << setw(5) << getL3CacheHitRatio(sstate1, sstate2) <<
	"   " << setw(5) << getL2CacheHits(sstate1, sstate2) <<
	"   " << setw(5) << getL2CacheHitRatio(sstate1, sstate2) << endl;

	m->cleanup();

}
