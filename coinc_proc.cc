
#include <iostream>
#include <iomanip>
#include <pmonitor.h>
//#include <PhHistogramFactory.hh>
#include <TH1F.h>
#include <TH2F.h>
#include <TTree.h>
#include <TBranch.h>

//PhHistogramFactory *hf;

#include <set>

int init_done = 0;

TH1F *h1; 
TH1F *hf1; 

TH1F *h2; 
TH1F *hf2; 

TH1F *h3; 
TH1F *hf3; 
TH1F *hftruediff; 

TH1F *h4; 

TH1F *hblocks, *hids;

TH2F *hpvp;

TH2F *block2d;

#include "pet_datum.h"
#include <stdio.h>
#include <fstream>
#include <map>

typedef std::set<pet_datum * , petTimeSort> datumset;
typedef datumset::iterator datumiter;
datumset TheSet;

#define REQUIRED_LIST_LENGTH 1000
#define MAX_TIME_DIFF 6
#define DELAY_AMOUNT 27


TFile *hfile;

unsigned short sinogram[72192];
unsigned short delayedsinogram[72192];


std::map <int, int> *pmap;


int fill_map()
{

  pmap = new std::map <int, int>;


  std::fstream IN ("sinoindex.txt", std::fstream::in );
  
  int key;

  int index=0;

  while ( IN>>key )
    {
 
      (*pmap)[index++] = key;
    }
  std::cout << "---------------------" << std::endl;
  return 0;
}

int update_sino ( const int bid1, const int cid1, const int bid2, const int cid2, const double diff)
{
  int i;
  int key;

  if ( bid1 < bid2)
    {
      key = bid1*32*32*12 + bid2*32*32 + cid1*32 + cid2;
    }
  else if ( bid1 > bid2 )
    {
      key = bid2*32*32*12 + bid1*32*32 + cid2*32 + cid1;
    }
  else
    {
      if ( cid1 < cid2)
	{
	  key = bid1*32*32*12 + bid2*32*32 + cid1*32 + cid2;
	}
      else
	{
	  key = bid1*32*32*12 + bid2*32*32 + cid2*32 + cid1;
	}
	  
    }

  int index = (*pmap)[key];
  if ( index < 0)
    {

    }

  if ( index <0 || index >=72192)
    {
      std::cout << __LINE__ << "  Index out of range: " << index << std::endl;
      return -1;
    }
  
  //  std::cout << " coincidence: b1 c1 b2 c2: " << bid1 << "  " << cid1 << "  " << bid2 << "  " << cid2 << 
  //   " key: " << key << " index: " << index << std::endl;

  sinogram[index]++;
  
  FILE * fprompts = fopen("prompts.txt", "a");
  
  if (!fprompts) 
    {
      std::cout << "could not open write file"  << std::endl;
      return -1;
    }
  
 
  fprintf(fprompts, "%d   %d   %d   %d\n",bid1,bid2,cid1,cid2); 
  fclose(fprompts);
  
 

 
  double sum = 0;
  for (i = 0; i< 72192; i++) sum += sinogram[i];
  
  //  std::cout << "number of coincidences: " << sum << std::endl;
  //return 0;
  

  return 0;
}

int update_delayedsino ( const int bid1, const int cid1, const int bid2, const int cid2, const double diff)
{
  int key;

  if ( bid1 < bid2)
    {
      key = bid1*32*32*12 + bid2*32*32 + cid1*32 + cid2;
    }
  else if ( bid1 > bid2 )
    {
      key = bid2*32*32*12 + bid1*32*32 + cid2*32 + cid1;
    }
  else
    {
      if ( cid1 < cid2)
	{
	  key = bid1*32*32*12 + bid2*32*32 + cid1*32 + cid2;
	}
      else if  (cid1 > cid2)
	{
	  key = bid1*32*32*12 + bid2*32*32 + cid2*32 + cid1;
	}
	  
      else 
	{
	  return 0;
	}
    }

  int index = (*pmap)[key];
  if ( index < 0)
    {

    }

  if ( index <0 || index >=72192)
    {
      std::cout << __LINE__ << "  Index out of range: " << index << "  " 
		<< bid1 << "  " << cid1 << "  " << bid2 << "  " << cid2 << std::endl;
      return -1;
    }
  
  //  std::cout << " delayed coincidence: b1 c1 b2 c2: " << bid1 << "  " << cid1 << "  " << bid2 << "  " << cid2 << 
  // " key: " << key << " index: " << index << " diff: " << diff << std::endl;
  delayedsinogram[index]++;
  return 0;
}



int write_sinogram( const char *filename)
{
  FILE * fp = fopen( filename, "w");
   
  if (!fp) 
    {
      std::cout << "could not open " << filename << std::endl;
      return -1;
    }

  
  fwrite( sinogram, 2, 72192, fp);
  fclose(fp);

  int i;
  double sum = 0;
  for (i = 0; i< 72192; i++) sum += sinogram[i];

  std::cout << "number of coincidences: " << sum << std::endl;
  return 0;

}

int write_delayedsinogram( const char *filename)
{
  FILE * fp = fopen( filename, "w");
   
  if (!fp) 
    {
      std::cout << "could not open " << filename << std::endl;
      return -1;
    }

  
  fwrite( delayedsinogram, 2, 72192, fp);
  fclose(fp);
  int i;
  double sum = 0;
  for (i = 0; i< 72192; i++) sum += delayedsinogram[i];

  std::cout << "number of delayed coincidences: " << sum << std::endl;
  return 0;

}


int clear_sinogram()
{
  int i;
  for ( i = 0; i <  72192; i++) 
    {
      sinogram[i] =0;
      delayedsinogram[i] = 0;
    }
  return 0;
}


int pinit()
{
  
  if (init_done) return 1;
  init_done = 1;
  hfile = 0;
  clear_sinogram();

  fill_map();

  //  h1 = hf->CreateH1F("H1", "Channel 0", 101,-49.5,49.5);

  h1 = new TH1F( "ctd", "closest time distribution", 256,0, 5000000);
  hf1 = new TH1F( "ctdf", "closest time distribution", 256,0, 500);

  h2 = new TH1F( "atd", "all time distribution", 256,0, 5000000);
  hf2 = new TH1F( "atdf", "all time distribution", 256,0, 500);

  //  h3 = new TH1F( "rtd", "time distribution in window",256,0,MAX_TIME_DIFF+500  );
  hf3 = new TH1F( "rtdf", "time distribution in window",100,-0.5,99.5  );
  //  hftruediff = new TH1F( "rttruef", "time distribution in window for Paul",201,-100.5,100.5  );

  h4 = new TH1F( "nop", "number of partners", 8,-0.5, 7.5);

  hblocks = new TH1F( "hblocks", "fired block distribution", 12,-0.5, 11.5);
  hids = new TH1F( "hids", "fired crystal id distribution", 12*32,-0.5, 12*32+0.5);

  hpvp= new TH2F( "hpvp", "1st fired block vs 2nd", 
		  13, -0.5, 12.5,
		  13, -0.5, 12.5 );

  block2d= new TH2F( "block2d", "block distribution", 
		     12*4+22, -0.5, 12*4+21.5,
		     8, -0.5, 7.5);

 

  return 0;
}




//void opentree(const char *filename)
//{
//  hfile = new TFile("Event.root","RECREATE","Coincidences");
//  hfile->SetCompressionLevel(1);



void findCoincidence()
{

  datumiter itstart; // the first element that we are working on
  datumiter it;      // just an iterator through the rest
  int xxx = 0;

  while ( TheSet.size()  > REQUIRED_LIST_LENGTH  )   // wait until enough values are here
    {
      //      std::cout << TheSet.size() << std::endl;
      it = itstart = TheSet.begin();
      ++it;


      int first = 1;

      int cont=1;
      double diff;

      for ( ; (it != TheSet.end() ) && cont; ++it)
	{
	  // now if the time diff is beyond a certain threshold (10 here), end the inner loop
	  //	  int prec = std::cout.precision();
	  //	  std::cout << std::setprecision(24) << (*it)->getTimeStamp() << std::setprecision(prec) <<std::endl;
	  diff = (*it)->getTimeStamp() -  (*itstart)->getTimeStamp();
	  //if (diff < 50)  std::cout << "diff is " << diff << std::endl;

	  if ( first)
	    {
	      first = 0;
	      h1->Fill(diff);
	      hf1->Fill(diff);
	    }
	  h2->Fill(diff);
	  hf2->Fill(diff);

	  if ( diff <  MAX_TIME_DIFF ) 
	    {
	      (*itstart)->addPartner( **it);
	    }
	  else if ( diff > DELAY_AMOUNT && diff < DELAY_AMOUNT +  MAX_TIME_DIFF ) 
	    {
	      (*itstart)->addDelayedPartner( **it);
	    }
	  else
	    {
	      cont = 0;
	    }
	}

      if ( (*itstart)->getNrDelayedPartners() ==1 &&  (*itstart)->getDelayedOldPartners() == 0 )
	//if ( (*itstart)->getDelayedPartner() == 0 && (*itstart)->getOldPartners() == 0 )
	{

	  //  if ( (*itstart)->getDelayedPartner().getNrDelayedPartners() == 0   && (*itstart)->getDelayedPartner().getNrPartners() ==0 )
	  //  if ( (*itstart)->getDelayedPartner().getNrPartners() ==0 )
	    {
	      //	      std::cout << "found delayed match: " 
	      //		<<  (*itstart)->getBlockID() << "   "  
	      //		<<  (*itstart)->getCrystalID() << "   "
	      //		<<  (*itstart)->getDelayedPartner().getBlockID() << "   "  
	      //		<<  (*itstart)->getDelayedPartner().getCrystalID() << "   " 
	      //		<<  ((*itstart)->getDelayedPartner() ).getTimeStamp() -  (*itstart)->getTimeStamp()  << std::endl;

		update_delayedsino ( (*itstart)->getBlockID() ,  (*itstart)->getCrystalID(),
				     (*itstart)->getDelayedPartner().getBlockID() ,  
				     (*itstart)->getDelayedPartner().getCrystalID(),
				     (*itstart)->getDelayedPartner().getTimeStamp() - (*itstart)->getTimeStamp() );
	    }
	}



      if ( (*itstart)->getNrPartners() ==1 &&  (*itstart)->getOldPartners() == 0)
      	{ 

       if ( (*itstart)->getPartner().getNrPartners() == 0)
          {
	      // if ( (xxx++ % 1000 ) == 0)
	      //  {
	      //	      std::cout << "found match: " 
	      //		<<  (*itstart)->getBlockID() << "   "  
	      //		<<  (*itstart)->getCrystalID() << "   "
	      //		<<  (*itstart)->getPartner().getBlockID() << "   "  
	      //		<<  (*itstart)->getPartner().getCrystalID() << "   " 
	      //		<<  ((*itstart)->getPartner() ).getTimeStamp() -  (*itstart)->getTimeStamp()
	      //		<< std::endl;
	      //  }
      
	      update_sino ( (*itstart)->getBlockID() ,  (*itstart)->getCrystalID(),
			    (*itstart)->getPartner().getBlockID() ,  (*itstart)->getPartner().getCrystalID(),
			    (*itstart)->getPartner().getTimeStamp() - (*itstart)->getTimeStamp() );


	      double dd = ((*itstart)->getPartner() ).getTimeStamp() -  (*itstart)->getTimeStamp();
     

	      //	      h3->Fill ( dd );
	      hf3->Fill (dd );
	      //	      if ( ((*itstart)->getPartner() ).getBlockID() >  (*itstart)->getBlockID() ) dd *= -1;
	      //	      hftruediff->Fill ( dd);
	      //	      std::cout << "diff is " << dd << std::endl;
	      if  ( (*itstart)->getBlockID() <  (*itstart)->getPartner().getBlockID() )
		{
		  hpvp->Fill((*itstart)->getBlockID(),  (*itstart)->getPartner().getBlockID() );
		}
	      else
		{
		  hpvp->Fill( (*itstart)->getPartner().getBlockID() , (*itstart)->getBlockID() );
		}
		
	       }


	}
      h4->Fill( (*itstart)->getNrPartners() );
      pet_datum *ptr =  (*itstart);
      TheSet.erase( itstart); 
      delete ptr;
    }
}




int process_event (Event * e)
{
  //  static int i = 0;
  if ( e->getEvtSequence() < 3) return 0;

  pet_datum *pd;

  Packet *p = e->getPacket(1010);
  if (p) 
    {
      //p->dump();
      int is = p->iValue(0,"SAMPLES");
      
      int i;
      for ( i = 512; i < is; i++)
	{
	  
	  int bid = p->iValue(i,"BLOCKID");
	  int cid = p->iValue(i,"CRYSTALID");
	  if ( bid > 12 || bid < 0 || cid >32 || cid < 0) 
	    {
	      delete p;
	      return 0;
	    }

	  
	  int x2 = bid*4 + (cid/8) + 2*bid;
	  int y2 = (cid %8 );
	  block2d->Fill(x2,y2);

	  //      int prec = std::cout.precision();
	  unsigned long long x =  p->dValue(i);
	  //	      double t = (x & 0xf);
	  //x &= 0xfffffffffffffff0ll;
	  //double z = x;
	  //z += t* (15./9.);
	  //	      pd = new pet_datum ( z, bid, cid);
	  
	  x >>=4;
	  //	      int prec = std::cout.precision();
	  //std::cout << i << "   " << std::setprecision(24) << x << std::setprecision(prec) <<std::endl;
	  pd = new pet_datum ( x, bid, cid);
	  TheSet.insert(pd);
	  hblocks->Fill(bid);
	  hids->Fill(bid*32+cid);
	}
      findCoincidence();
      delete p;
    }

  return 0;
}


#define FIFO_ALMOST_FULL_VALUE (256*1024)
static unsigned int array[ FIFO_ALMOST_FULL_VALUE ];

unsigned int i4swap(const unsigned int in)
{
  union
  {
    unsigned int i4;
    char c[4];
  } i,o;
  
  i.i4 = in;
  o.c[0] = i.c[3];
  o.c[1] = i.c[2];
  o.c[2] = i.c[1];
  o.c[3] = i.c[0];
  return o.i4;
}

int readquickdaqfile (const char *filename)
{

  int total = 0;
  int i;
  FILE *fp = fopen( filename, "r");
  if ( !fp)
    {
      std::cout << "error opening " << filename  << std::endl;
      return -1;
    }


  pet_datum *pd;

  int status = fread (array, 4,  FIFO_ALMOST_FULL_VALUE , fp);
  while ( status > 0)
    {
      unsigned int last = 0;
      int j = 0;
      for (i=0; i< status; i++)
	{
	  array[j] = i4swap(array[i]);
	  if ( array[j] != last)
	    {
	      last = array[j];
	      j++;
	    }
	}
      status = j;
      
      int cid, bid;
      unsigned long long timestamp;
      
      for (i=0; i< status; i+=2)
	{
	  if ( (( array[i] >> 28 ) & 0xf) == 0xf)
	    {
	      i++;
	    }
	  bid = (array[i+1] >> 24) & 0xf;
	  cid = (array[i+1] >> 19) & 0x1f;
	  timestamp = array[i+1]  & 0x7ff;
	  timestamp = timestamp << 32;
	  timestamp += array[i];
	  
	  pd = new pet_datum ( timestamp, bid, cid);
	  total++;
	  TheSet.insert(pd);
	  hblocks->Fill(bid);
	  hids->Fill(bid*32+cid);
	}
      findCoincidence();
      status = fread (array, 4,  FIFO_ALMOST_FULL_VALUE , fp);
     
    }

  std::cout << "Total single count: " << total << std::endl; 
  return 0;
 
}





