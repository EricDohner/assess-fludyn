#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "flux_corrected_transport.h"

void read_params(double *dx, double *dt, double *x0, double *t0, double *v0, double *rho0i, int *Nx, int *Nt);
void initialize_array(double *arr, double cellwidth, int Ncells, double initial_value);
void array_multiply(double md[], double rho[], double vel[], int length);
void array_divide(double *num, double *denom, double *output, int length);
void array_clean(double arr[], int length, double cutoff);


/* 
 * The operator splitting procedure here is adapted from 
 * Boris and Book 1971, Flux Corrected Transport. I. SHASTA, A Fluid Transport Algorithm That Works, NRL, Washington
 * */

int main(void){

    
//     Initialize variables and arrays
    int i;
    int j;
    int i_t;
    int Ncells;
    double dx;
    double dt;
    int Nx;
    int Nt;
    double x0;
    double t0;
    double v0;
    double rho0i;
    
    read_params(&dx, &dt, &x0, &t0, &v0, &rho0i, &Nx, &Nt);
    
    double dt12 = dt*0.5;
    
    double xgrid[Nx];
    double tgrid[Nt];
    double rho[Nx];
    double rho0[Nx];
    double rho12[Nx];
    double vel[Nx];
    double vel12[Nx];
    double md[Nx];
    double md12[Nx];
    double md0[Nx];
    double cutoff = 10E-6;
    
    initialize_array(xgrid, dx, Nx, x0);
    initialize_array(tgrid, dt, Nt, t0);
//     initialize_array(rho12, dx, Nx, x0);
//     initialize_array(vel12, dx, Nx, x0);
    
    printf("tmax = %lf\n", tgrid[Nt-1]);
    
//     Initialize initial density and velocity arrays
    
    for(i = 0; i<Nx; i++){
     
        
            
        
        if(i > 25 && i < 125){
            rho[i] = rho0i;
            vel[i] = v0;
        
        }
        
    }
    
        char filename[30];
        sprintf(filename, "rho_t0.dat");
//         printf("%lf \n", tgrid[i_t]);
        
//         printf("%s \n", filename);
        

            FILE* rhoout = fopen(filename, "w");
            
            for(i = 0; i<Nx; i++){
            
                fprintf(rhoout, "%lf %lf %lf\n", xgrid[i], rho[i], vel[i]);

            }
            fclose(rhoout);
    
//     In an analogous procedure to Strang splitting, we're going to solve *both* continuity equations (momentum density and density)
//     for half a time step to get a midway value for the velocities (at half a time step ahead), then use those velocities
//     to evolve the original momentum and momentum density a full time step ahead. As mentioned above, this procedure
//      is adapted from Boris and Book, 1971.

            
    for(i_t = 0; i_t < Nt; i_t++){ //for each timestep...
    
        array_multiply(vel, rho, md, Nx); //construct the momentum density array
        
        array_clean(md, Nx, cutoff); //ensure we're not propagating momentum( density) values greater than 10E-6
        array_clean(rho, Nx, cutoff);
        
        flux_corr_method(md, vel, Nx, dt12, dx, md12); //advect momentum density, md, for 0.5*dt
        flux_corr_method(rho, vel, Nx, dt12, dx, rho12); //ditto for density, rho
        
        array_clean(rho12, Nx, cutoff); //as above
        array_clean(md12, Nx, cutoff);
        
        array_divide(md12, rho12, vel12, Nx); //vel12 now holds the velocity for half a time step ahead
        
        flux_corr_method(rho, vel12, Nx, dt, dx, rho0);
        flux_corr_method(md, vel12, Nx, dt, dx, md0); 
        //Momentum density and density have now been propagated a full time step, into the arrays rho0 and md0

        array_divide(md0, rho0, vel, Nx); //new velocity = new momentum density / new density
        
        for(i = 0; i<Nx; i++){ //make sure the next timestep uses the right values...
            
            rho[i] = rho0[i];
            md[i] = md0[i];
                
        }
        

        
        if(i_t % 10 == 0 && tgrid[i_t] < 10.0){ //every few timesteps output a new file with the rho and v values up to t = 10s
            
        sprintf(filename, "outdat/%lf", tgrid[i_t]);
        rhoout = fopen(filename, "w");
            
            fprintf(rhoout, "%lf #time\n", tgrid[i_t]);
            fprintf(rhoout, "x(m), rho(au), v(m/s)\n");
            
            for(i = 0; i<Nx; i++){
                
                fprintf(rhoout, "%lf %lf %lf\n", xgrid[i], rho[i], vel[i]);
                
            }
            
            
        fclose(rhoout);
            

            
        }
    
    
    }
    
    return 0;
}

void array_clean(double arr[], int length, double cutoff){
    
    int i;
    
    for(i = 0; i<length; i++){
     
        if(arr[i] <= cutoff){arr[i] = 0.0;}
        
    }
    
    return;
    
}

void array_multiply(double in1[], double in2[], double output[], int length){
 
    int i;
    
    for(i = 0; i<length; i++){
     
        output[i] = in1[i]*in2[i];
        
    }
    
    return;
    
}

void array_divide(double num[], double denom[], double output[], int length){
 
    int i;
    
    double cutoff = 10E-12;
    
    for(i = 0; i<length; i++){
        
        output[i] = 0;
     
        if(denom[i] > cutoff){output[i] = num[i]/denom[i];}
        
    }
    
    return;
    
}

void initialize_array(double *arr, double cellwidth, int Ncells, double initial_value){
    
    int i;
    
    for(i = 0; i<Ncells; i++){
     arr[i] = initial_value + cellwidth*(i + 0.5);
//      printf("%lf\n", arr[i]);
    }
    return;
}

void read_params(double *dx, double *dt, double *x0, double *t0, double *v0, double *rho0i, int *Nx, int *Nt){

    FILE* inputs = fopen("inputs.txt", "r");
    if(inputs == NULL){perror("inputs.txt does not exist");}
    
    char inputstring[200];
    
    if( fgets (inputstring, 200, inputs)!=NULL ) 
    {
      sscanf(inputstring, "%lf %lf %lf %lf %lf %lf %d %d", dx, dt, x0, t0, v0, rho0i, Nx, Nt);
    }
    
    if((*v0)*(*dt)/(*dx) >= 0.5){perror("CFL condition not satisfied!");}
    
    return;
    
}
