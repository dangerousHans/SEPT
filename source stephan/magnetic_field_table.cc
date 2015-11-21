
#define _SVID_SOURCE
#include <stdio.h>

#include "magnetic_field.hh"
#include "magnet_geometry.hh"
#include <G4FieldManager.hh>
#include <G4TransportationManager.hh>
#include <globals.hh>
#include <math.h>

magnetic_field_table::magnetic_field_table(const char *filename, 
					   double x,
					   double y,
					   double z) 
{
  origin[0] = x;
  origin[1] = y;
  origin[2] = z;
  
  FILE *f;
  if (filename[0] == '|')
    f = popen(filename+1, "r");
  else
    f = fopen(filename, "r");
  if (!f)
    {
      perror(filename);
      exit(1);
    }
  
  if (6 != fscanf(f, 
		  "%u%lf%u%lf%u%lf",
		  &steps[0], &range[0],
		  &steps[1], &range[1],
		  &steps[2], &range[2]))
    {
      perror(filename);
      exit(1);
    }

  int i;
  for (i=0; i<3; i++)
    step[i] = range[i]/(steps[i]-1);
  
  unsigned ts = steps[0]*steps[1]*steps[2];
  
  if (ts>0x1000000)
    {
      fprintf(stderr, "Warning: large Bfield table: %u\n", ts);
    }

  double (*tt)[3] = new double[ts][3];
  table = tt;

  unsigned tc = 0;

  double p[3],b[3];

  while (6==fscanf(f, 
		   "%lf%lf%lf%lf%lf%lf", 
		   &p[0],&p[1],&p[2],
		   &b[0],&b[1],&b[2]))
    {
      int ix[3];

      for (i=0; i<3; i++)
	{
	  ix[i] = int(floor(p[i]/step[i]+0.5));
	  if (ix[i] < 0 || ix[i] >= (int)steps[i]) 
	    {
	      fprintf(stderr, 
		      "Bfield Coordinates out of range: %G,%G,%G",
		      p[0],p[1],p[2]);
	      exit(1);
	    }
	}

      for (i=0; i<3; i++)
	{
	  tt[idx(ix)][i] = b[i]*tesla;
	}

      tc ++;
    }

  if (tc != ts)
    {
      perror(filename);
      exit(1);
    }

  if (filename[0] == '|')
    pclose(f);
  else
    fclose(f);

  G4FieldManager* fieldMgr =
    G4TransportationManager::GetTransportationManager()->GetFieldManager();
  
  fieldMgr->SetDetectorField(this);
  fieldMgr->CreateChordFinder(this); // TODO What's this ????
}


magnetic_field_table::~magnetic_field_table() 
{
  delete[] table;
}

void magnetic_field_table::GetFieldValue(const G4double xx[],
					 G4double B[3]) const 
{
  B[0] = B[1] = B[2] = 0;

  int sign[3] = { 1,1,1 };

  int ix[3];
  double w[3][2], x[3];

  int i;

  for (i=0; i<3; i++)
    {
      x[i] = (xx[i] - origin[i])/mm;

      if (x[i] < 0)
	{
	  x[i] = fabs(x[i]);

	  if (fabs(x[i]) >= range[i])
	    return;

	  if (i==1)
	    sign[0] = -sign[0];

	  if (i==0)
	    sign[1] = -sign[1];

	  sign[2] = -sign[2];
	}
      
      w[i][1] = x[i]/step[i];
      ix[i] = int(floor(w[i][1]));
      w[i][1] -= ix[i];
      w[i][0] = 1 - w[i][1];

      if (ix[i]+1 >= (int)steps[i])
	return;
    }

  for (int ii=0; ii<2; ii++)
    for (int jj=0; jj<2; jj++)
      for (int kk=0; kk<2; kk++)
	for (i=0; i<3; i++)
	  {
	    B[i] += w[0][ii] * w[1][jj] * w[2][kk]
	      * get(ix[0]+ii, ix[1]+jj, ix[2]+kk, i);
	  }
  
  for (i=0; i<3; i++)
    {
      B[i] *= sign[i];
    }

#if 0
  fprintf(stderr, "B(%G,%G,%G)=(%G,%G,%G)\n",
	  xx[0],xx[1],xx[2], B[0],B[1],B[2]);
#endif

  return;
}

