//
// A demonstration of using the KDTREE2 C++ routines, and timing.
// This file is in the public domain.
//

#include "kdtree2.hpp"

#include <boost/multi_array.hpp>
#include <boost/random.hpp>

static boost::minstd_rand generator(42u); 
static boost::uniform_real<> uni_dist(0,1); 
boost::variate_generator<boost::minstd_rand&,boost::uniform_real<> > uni(generator,uni_dist); 

float random_variate() {
  // between [0,1)
  return(uni()); 
}

//
// define, for convenience a 2d array of floats. 
//
typedef boost::multi_array<float,2> array2dfloat;
typedef boost::multi_array<float,1> array1dfloat;


#include <ctime>

float time_a_search(kdtree2::KDTree* tree, int nn, int nsearch) {
  int dim = tree->dim;
  std::vector<float> query(dim);
  kdtree2::KDTreeResultVector result; 

  clock_t t0, t1; 

  t0 = clock();

  for (int i=0; i<nsearch;i++) {
      for (int j=0; j<dim; j++) query[j] = random_variate(); 
      tree->n_nearest(query,nn,result);
  }

  t1 = clock(); 

  return(static_cast<float> 
	 (static_cast<double> (t1-t0) / static_cast<double> (CLOCKS_PER_SEC) ));
}

void time_random_searches(kdtree2::KDTree* tree, int nn) {
  // emit the number of searches per second.
  int nsearch;

  nsearch = 50;
  for(;;) {
    float t = time_a_search(tree,nn,nsearch); 
    if (t < 1.0) {
      nsearch *= 5; 
      continue; 
    } else {
      float sps = float(nsearch) / t ;
      std::cout << "C++ impl, for nn=" << nn << " searches/sec = " << sps << "\n";
      return;
    }
  }
}

int main() {
  array2dfloat data(boost::extents[10][3]);  // declare a 10000 x 3 array.
  array2dfloat realdata; 
  array1dfloat xper(boost::extents[3]); 
  array1dfloat realxper; 

  // notice it is in C-standard layout. 
  kdtree2::KDTree* tree;
  kdtree2::KDTreePeriod* period;
  kdtree2::KDTreeResultVector res; 
  int N, dim;

  if (false) {
    for (int i=0; i<10; i++) {
      for (int j=0; j<3; j++)
	data[i][j] = static_cast<float> (3*i+j);
    }
      for (int j=0; j<3; j++)
	xper[j] = static_cast<float> (1.0);
    tree = new kdtree2::KDTree(data,xper,true); 
    
    //    tree->dump_data(); 
    //data[0][0]=666.0;  // mutate it underneath.  DO NOT DO THIS IN REAL USE
    //tree->dump_data(); // test to see it change there.
    
    tree->n_nearest_around_point(1,1,1,res); 
    for (unsigned int i=0; i<res.size(); i++) {
      printf("result[%d]= (%d,%f)\n",i,res[i].idx,res[i].dis);
    }
    
    delete tree;
  }
  printf("Give me N, and dim (e.g. '1000 3').  No commas!");
  scanf("%d %d",&N,&dim);
  printf("I found N=%d,dim=%d\n",N,dim);
  realdata.resize(boost::extents[N][dim]); 
  realxper.resize(boost::extents[dim]); 
    
  for (int i=0; i<N; i++) {
    for (int j=0; j<dim; j++)
      realdata[i][j] = random_variate();
  }
      for (int j=0; j<3; j++)
	realxper[j] = static_cast<float> (1.0);

  tree = new kdtree2::KDTree(realdata,realxper,true);
  tree->sort_results = true;
  std::cout << "Tree created, now testing against brute force..."; 
  {
    std::vector<float> query(dim); 
    kdtree2::KDTreeResultVector result, resultbrute;
    int nn = 10; 

    for (int i=0; i<50; i++) {
      for (int j=0; j<dim; j++) query[j] = random_variate(); 

      tree->n_nearest_brute_force(query,nn,resultbrute);
      tree->n_nearest(query,nn,result); // search for 10 of them.

      for (int k=0; k<nn; k++) {
	if ((resultbrute[k].dis != result[k].dis) ||
	    (resultbrute[k].idx != result[k].idx)) {
	  std::cout << "Mismatch! nn=" << k << " brute=[" << 
	    resultbrute[k].dis << "," << resultbrute[k].idx << 
	    "] tree=[" << result[k].dis << "," << result[k].idx << "]\n"; 
	}
      }
    }
  }
  std::cout << "\nTesting complete.  Now testing timing...\n";
  tree->sort_results = false;
  
  {

    int nnarray[] = {1,5,10,25,500} ;
    for (int i=0; i< 5; i++) {
      time_random_searches(tree,nnarray[i]); 
    }
    
  }

  /* Tests for the count routine
  {

    std::vector<float> query(dim);
    float rrarray[] = {0.05,0.10,0.15,0.2} ;
    for (int i=0; i< 4; i++) {
      for (int j=0; j<dim; j++) query[j] = random_variate(); 
	int count=tree->r_count(query,rrarray[i]);
	for(int j=0;j<dim;j++)
	    std::cout<<query[j]<<" ";
	std::cout<<count<<std::endl;
    }
    
  }
  */


}

