# include <cstdlib>
# include <iostream>
# include <iomanip>
# include <cmath>
# include <ctime>
# include <fstream>

using namespace std;

int main ( );
double f ( double x );
double g ( double x );
void timestamp ( );

int main ( )
{
# define m 30
# define n 40

  double alpha;
  double c = 0.25;
  ofstream command_unit;
  ofstream data_unit;
  double dt;
  double dx;
  int i;
  int j;
  double t;
  double t1 = 0.0;
  double t2 = 3.0;
  double u[m+1][n+1];
  double x;
  double x1 = 0.0;
  double x2 = 1.0;

  timestamp ( );
  cout << "\n";
  cout << "STRING_SIMULATION:\n";
  cout << "  C++ version\n";
  cout << "  Simulate the behavior of a vibrating string.\n";

  dx = ( x2 - x1 ) / ( double ) n;
  dt = ( t2 - t1 ) / ( double ) m;
  alpha = pow ( c * dt / dx, 2 );
  cout << "  ALPHA = ( C * dT / dX )^2 = " << alpha << "\n";
//
//  Warn the user if ALPHA will cause an unstable computation.
//
  if ( 1.0 < alpha )
  {
    cout << "\n";
    cout << "  Warning!\n";
    cout << "  ALPHA is greater than 1.\n";
    cout << "  The computation is unstable.\n";
  }
//
//  Time step 0: 
//  Use the initial condition for U.
//
  u[0][0] = 0.0;
  for ( j = 1; j < n; j++ )
  {
    x = j * dx;
    u[0][j] = f ( x );
  }
  u[0][n] = 0.0;
//
//  Time step 1:
//  Use the initial condition for dUdT.
//
  u[1][0] = 0.0;
  for ( j = 1; j < n; j++ )
  {
    x = j * dx;
    u[1][j] = 
        ( alpha / 2.0 ) * u[0][j-1]
      + ( 1.0 - alpha ) * u[0][j] 
      + ( alpha / 2.0 ) * u[0][j+1]
      + dt * g ( x );
  }
  u[1][n] = 0.0;
//
//  Time steps 2 through M:
//
  for ( i = 2; i <= m; i++ )
  {
    u[i][0] = 0.0;
    for ( j = 1; j < n; j++ )
    {
      u[i][j] = 
                        alpha   * u[i-1][j-1]
        + 2.0 * ( 1.0 - alpha ) * u[i-1][j] 
        +               alpha   * u[i-1][j+1]
        -                         u[i-2][j];
    }
    u[i][n] = 0.0;
  }
//
//  Write data file.
//
  data_unit.open ( "string_data.txt" );

  for ( i = 0; i <= m; i++ )
  {
    t = i * dt;
    for ( j = 0; j <= n; j++ )
    {
      x = j * dx;
      data_unit << "  " << x
                << "  " << t
                << "  " << u[i][j] << "\n";
    }
    data_unit << "\n";
  }
  data_unit.close ( );

  cout << "\n";
  cout << "  Plot data written to the file \"string_data.txt\".\n";
//
//  Write gnuplot command file.
//
  command_unit.open ( "string_commands.txt" );

  command_unit << "set term png\n";
  command_unit << "set output \"string.png\"\n";
  command_unit << "set grid\n";
  command_unit << "set style data lines\n";
  command_unit << "unset key\n";
  command_unit << "set xlabel '<---X--->'\n";
  command_unit << "set ylabel '<---Time--->'\n";
  command_unit << "splot \"string_data.txt\" using 1:2:3 with lines\n";
  command_unit << "quit\n";

  command_unit.close ( );

  cout << "  Gnuplot command data written to the file \"string_commands.txt\".\n";
//
//  Terminate.
//
  cout << "\n";
  cout << "STRING_SIMULATION:\n";
  cout << "  Normal end of execution.\n";
  cout << "\n";
  timestamp ( );

  return 0;
# undef m
# undef n
}
//****************************************************************************80

double f ( double x )

//****************************************************************************80
//
//  Purpose:
//
//    F supplies the initial condition.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    01 December 2012
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the location.
//
//    Output, double F, the value of the solution at time 0 and location X.
//
{
  double value;

  if ( 0.25 <= x && x <= 0.50 )
  {
    value = ( x - 0.25 ) * ( 0.50 - x );
  }
  else
  {
    value = 0.0;
  }

  return value;
}
//****************************************************************************80

double g ( double x )

//****************************************************************************80
//
//  Purpose:
//
//    G supplies the initial derivative.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    01 December 2012
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, the location.
//
//    Output, double G, the value of the time derivative of the solution 
//    at time 0 and location X.
//
{
  double value;

  value = 0.0;

  return value;
}
//****************************************************************************80

void timestamp ( )

//****************************************************************************80
//
//  Purpose:
//
//    TIMESTAMP prints the current YMDHMS date as a time stamp.
//
//  Example:
//
//    31 May 2001 09:45:54 AM
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    01 December 2012
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    None
//
{
# define TIME_SIZE 40

  static char time_buffer[TIME_SIZE];
  const struct std::tm *tm_ptr;
  std::time_t now;

  now = std::time ( NULL );
  tm_ptr = std::localtime ( &now );

  std::strftime ( time_buffer, TIME_SIZE, "%d %B %Y %I:%M:%S %p", tm_ptr );

  std::cout << time_buffer << "\n";

  return;
# undef TIME_SIZE
}