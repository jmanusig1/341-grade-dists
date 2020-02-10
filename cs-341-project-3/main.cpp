/*main.cpp*/

//
// Name: Jared Manusig
// U. of Illinois, Chicago
// CS 341, Fall 2019
// Project #03: GradeUtil UI
// This program is C++11 dependent
//
// Program uses modern C++ programming to 
// Get grade distributions given CSV files
//

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <map> 
using namespace std;

// includes for gradeutil
#include "gradeutil.h"

College InputGradeData(string filename)
{
    College college;
    ifstream file(filename);
    string line, value;

    if (!file.good())
    {
        cout << "**Error: unable to open input file '" << filename << "'." << endl;
        return college;
    }

    // first line contains semester,year
    getline(file, line);
    stringstream ss(line);

    getline(ss, college.Name, ',');
    getline(ss, college.Semester, ',');
    getline(ss, value);
    college.Year = stoi(value);

    // second line contains column headers --- skip
    getline(file, line);

    //
    // now start inputting and parse course data:
    //

    while (getline(file, line))
    {
        Course c = ParseCourse(line);

        //
        // search for correct dept to ask course to, otherwise create a new dept:
        //
        auto dept_iter = std::find_if(college.Depts.begin(),
                                      college.Depts.end(),
                                      [&](const Dept &d) {
                                          return (d.Name == c.Dept);
                                      });

        if (dept_iter == college.Depts.end())
        {
            //
            // doesn't exist, so we have to create a new dept
            // and insert course:
            //
            Dept d(c.Dept);

            d.Courses.push_back(c);

            college.Depts.push_back(d);
        }
        else
        {
            // dept exists, so insert course into existing dept:
            dept_iter->Courses.push_back(c);
        }

    } //while

    //
    // done:
    //
    return college;
}


//function gives a summary of the grade distribution
//for the whole department 
void dept_summary(Dept d){
  
  //get courses and initialize students 
  int courses = d.Courses.size();
  int students = 0;
  
  //loop through the courses to get the number of students
  for(Course c : d.Courses){
    students += c.getNumStudents();
  }
  
  //format the output
  cout << d.Name << ":" << endl;
  cout << " # courses taught: " << courses << endl;
  cout << " # students taught: " << students << endl;
  
  //get the GradeDistribution 
  GradeStats s = GetGradeDistribution(d);
  
  //format the output for grade distribution
  cout << " grade distribution (A-F): " << s.PercentA << "%, ";
  cout << s.PercentB << "%, ";
  cout << s.PercentC << "%, ";
  cout << s.PercentD << "%, ";
  cout << s.PercentF << "%" << endl;
  
  //dfw = percent of students that got D, F, or Withdrew
  int dfw = 0;
  int n = 0;
  
  //output dfw rate
  cout << " DFW rate: " << GetDFWRate(d, dfw, n) << "%" << endl;
}

//function gives a summary for a certain course
void course_summary(Course c){
  
  //formatting output for course
  cout << c.Dept << " " << c.Number << " (section "; 
  cout << c.Section << "): " << c.Instructor << endl;
  cout << " # students: " << c.getNumStudents() << endl;
  
  //get grading types and output
  if(c.getGradingType() == 0){
    cout << " course type: letter" << endl;
  }
  else if(c.getGradingType() == 1){
    cout << " course type: satisfactory" << endl;
  }
  else{
    cout << " course type: unknown" << endl;
  }
  
  //format and output the grade distribution
  GradeStats s = GetGradeDistribution(c);
  cout << " grade distribution (A-F): " << s.PercentA << "%, ";
  cout << s.PercentB << "%, ";
  cout << s.PercentC << "%, ";
  cout << s.PercentD << "%, ";
  cout << s.PercentF << "%" << endl;
  
  //dfw = percent of students that got D, F, or Withdrew
  int dfw = 0;
  int n = 0;
  
  //output dfw rate
  cout << " DFW rate: " << GetDFWRate(c, dfw, n) << "%" << endl;
 
}


//function prints a grade distribution summary for a certain course
void printSummary(Course c){
  
  //get dfw rates
  int dfw = 0;
  int n = 0;
  
  //format outputs
  cout << c.Dept << " " << c.Number << " (section "; 
  cout << c.Section << "): " << c.Instructor << endl;
  cout << " # students: " << c.getNumStudents() << endl;
  
  //get the gradeing type and output
  if(c.getGradingType() == 0){
  cout << " course type: letter" << endl;
  }
  else if(c.getGradingType() == 1){
    cout << " course type: satisfactory" << endl;
  }
  else{
    cout << " course type: unknown" << endl;
  }
  
  //get the grade distribution and output
  GradeStats s = GetGradeDistribution(c);
  cout << " grade distribution (A-F): " << s.PercentA << "%, ";
  cout << s.PercentB << "%, ";
  cout << s.PercentC << "%, ";
  cout << s.PercentD << "%, ";
  cout << s.PercentF << "%" << endl;
  cout << " DFW rate: " << GetDFWRate(c, dfw, n) << "%" << endl;
}



//function outputs a summary of a certain college
void summary(College c, string input){
  
  //given the departments
  vector<Dept> depts = c.Depts;
    
  //sort the departments alphabetically using the sort function
  sort(depts.begin(), depts.end(),
      [] (const Dept a, const Dept b){
        return a.Name < b.Name;
      });
  
 
  if(input == "all"){
    if(depts.size() == 0){
      
      return;
      
    }
    //output each dept summary
    for(Dept d : depts){
      dept_summary(d);
    }
  }
  else{ 
    //otherwise use the iterator function to go through every department
    vector<Dept>::iterator it = find_if(depts.begin(), depts.end(), 
         [=](const Dept d){
           if(d.Name == input)
             return true;
           return false;
         });
    
    //otherwise department not found
    if(it == depts.end()){
      cout << "**dept not found" << endl;
    }
    else{
      dept_summary(it[0]);
    }
  }
}

//function searches for instructor
void search(College c, string dept_name, string insPre){
  
  //get the courses, input, and departments for a certain college
  vector<Course> courses;
  vector<Dept> depts = c.Depts;
  int courseNum;
  stringstream ss(insPre);
  ss >> courseNum;
  
  //if all, then find instructor for all courses
  if(dept_name == "all"){
    if(ss.fail()){
      courses = FindCourses(c, insPre);
    }
    else{
      courses = FindCourses(c, courseNum);
    }
  }
  else{
    //otherwise iterator through to find 
    vector<Dept>::iterator it = find_if(depts.begin(), depts.end(), 
       [=](const Dept d){
         if(d.Name == dept_name)
           return true;
         return false;
       });
    if(it == depts.end()){
      cout << "**dept not found" << endl;
      return;
    }
    else{
      if(ss.fail()){
        courses = FindCourses(it[0], insPre);
      }
      else{
        courses = FindCourses(it[0], courseNum);
      }
    }
  }
  if(courses.size() == 0){
        cout << "**none found" << endl;
        return;
      }
  
  for(Course c : courses){
    course_summary(c);
  }
  
}

//function finds out the number of U/S courses
void satisfactory(College c, string input){
  
  vector<Course> courses;
  
  if(input == "all"){
    for(Dept d : c.Depts){
      for(Course c : d.Courses){
        if(c.getGradingType() == 1)
          courses.push_back(c);
      }
    }
  }
  else{
    
    vector<Dept>::iterator it = find_if(c.Depts.begin(), c.Depts.end(), 
       [=](const Dept d){
         if(d.Name == input)
           return true;
         return false;
       });
    if(it == c.Depts.end()){
      cout << "**dept not found" << endl;
      return;
    }
    else{
      Dept dep = it[0];
    
      for(Course c : dep.Courses){
        if(c.getGradingType() == 1)
          courses.push_back(c);
      }
    }
    
  }
  
  //
  sort(courses.begin(), courses.end(),
    [](Course c1, Course c2){
      if(c1.Dept < c2.Dept)
          return true;
        else if(c1.Dept > c2.Dept)
          return false;
      else{
        if (c1.Number < c2.Number)
          return true;
        else if (c1.Number > c2.Number)
          return false;
        else // equal, determine by section number:
          if (c1.Section < c2.Section)
          return true;
        else 
          return false;
      }
      return false;
    });
  
  if(courses.size() == 0){
      cout << "**none found" << endl;
      return;
    }
  
  for(Course c : courses){
    cout << c.Dept << " " << c.Number << " (section "; 
    cout << c.Section << "): " << c.Instructor << endl;
    cout << " # students: " << c.getNumStudents() << endl;
    cout << " course type: satisfactory" << endl;
  }
}

void dfw_rate(College c, string input, double dfw_rate){
  vector<Course> courses;
  int dfw = 0;
  int n = 0;
  
 // map<string, double> cMap;
  
  if(input == "all"){
    for(Dept d : c.Depts){
      for(Course c : d.Courses){
        if(GetDFWRate(c, dfw, n) > dfw_rate)
          courses.push_back(c);
          //cMap.insert(pair<string, double>(c.Title, GetDFWRate(c, dfw, n)));
      }
    }
  }
  else{
    
    vector<Dept>::iterator it = find_if(c.Depts.begin(), c.Depts.end(), 
       [=](const Dept d){
         if(d.Name == input)
           return true;
         return false;
       });
    if(it == c.Depts.end()){
      cout << "**dept not found" << endl;
      return;
    }
    else{
    
      Dept dep = it[0];

      for(Course c : dep.Courses){
        if(GetDFWRate(c, dfw, n) > dfw_rate){
           courses.push_back(c);
           //cMap.insert(pair<string, double>(c.Title, GetDFWRate(c, dfw, n)));
        }
      }
    }
  }
  if(courses.size() == 0){
      cout << "**none found" << endl;
      return;
  }
  
  //TODO SORTING
  sort(courses.begin(), courses.end(), 
      [](Course c1, Course c2){
        int i, j;
        double dfw1 = GetDFWRate(c1, i, j);
        double dfw2 = GetDFWRate(c2, i, j);
        
        if(dfw1 > dfw2){
         return true;
        }
        if(dfw1 == dfw2){
          if(c1.Dept < c2.Dept)
            return true;
          else if(c1.Dept > c2.Dept)
            return false;
          else{
            if (c1.Number < c2.Number)
              return true;
            else if (c1.Number > c2.Number)
              return false;
            else // equal, determine by section number:
              if (c1.Section < c2.Section)
              return true;
            else 
              return false;
          }
        }
        return false;
      });
  
  for(Course c : courses){
    printSummary(c);
  }
  
}

void letterB(College c, string input, double b_rate){
  vector<Course> courses;
  
 //map<string, double> cMap;
  
  if(input == "all"){
    for(Dept d : c.Depts){
      for(Course c : d.Courses){
        if(c.getGradingType() == 0){
          GradeStats s = GetGradeDistribution(c);
          if(s.PercentB > b_rate){
            courses.push_back(c);
            //cMap.insert(pair<string, double>(c.Title, s.PercentB));
          }
        }
      }
    }
  }
  else{
    
    vector<Dept>::iterator it = find_if(c.Depts.begin(), c.Depts.end(), 
       [=](const Dept d){
         if(d.Name == input)
           return true;
         return false;
       });
    if(it == c.Depts.end()){
      cout << "**dept not found" << endl;
      return;
    }
    else{
      Dept dep = it[0];

      for(Course c : dep.Courses){
        GradeStats s = GetGradeDistribution(c);
        if(s.PercentB > b_rate){
          if(c.getGradingType() == 0)
            courses.push_back(c);
          //cMap.insert(pair<string, double>(c.Title, s.PercentB));
        }
      }
    }
  }
  
  //TODO SORTING
  sort(courses.begin(), courses.end(), 
      [](Course c1, Course c2){
        
        double b1 = GetGradeDistribution(c1).PercentB;
        double b2 = GetGradeDistribution(c2).PercentB;
        
        if(b1 > b2){
         return true;
        }
        if(b1 == b2){
          if(c1.Dept < c2.Dept)
            return true;
          else if(c1.Dept > c2.Dept)
            return false;
          else{
            if (c1.Number < c2.Number)
              return true;
            else if (c1.Number > c2.Number)
              return false;
            else // equal, determine by section number:
              if (c1.Section < c2.Section)
              return true;
            else 
              return false;
          }
        }
        return false;
      });
  
  if(courses.size() == 0){
        cout << "**none found" << endl;
        return;
   }
  
  for(Course c : courses){
    printSummary(c);
  }
}



void average(College college, string input){
  
  double gpa = 0;
  //double graded = 0;
  //double check = 0;
  double a, b, c, d;
  GradeStats s = GradeStats();
  
  vector<Dept> deps = college.Depts;
  
  if(input == "all"){
    
   
    
    sort(deps.begin(), deps.end(),
        [](Dept a, Dept b){
          if(a.Name < b.Name)
            return true;
          return false;
        });
    
    for(Dept dep: deps){
      gpa = 0;
      s = GetGradeDistribution(dep);
      a = s.NumA * 4.0;
      b = s.NumB * 3.0;
      c = s.NumC * 2.0;
      d = s.NumD;
      gpa = (a+b+c+d)/(double)s.N; 
      cout << "Overall GPA for " << dep.Name << " : " << gpa << endl;
    }
    
  }
  else{
   vector<Dept>::iterator it = find_if(deps.begin(), deps.end(), 
       [=](const Dept d){
         if(d.Name == input)
           return true;
         return false;
       });
    if(it == deps.end()){
      cout << "**dept not found" << endl;
      return;
    }
    else{
      for(Course co: it[0].Courses){
        if(co.getGradingType() == 0){
        gpa = 0;
        s = GetGradeDistribution(co);
        a = s.NumA * 4.0;
        b = s.NumB * 3.0;
        c = s.NumC * 2.0;
        d = s.NumD;
        gpa = (a+b+c+d)/(double)s.N; 
        cout << "Overall GPA for " << co.Dept << " " << co.Number;
        cout << "(" << co.Section << ") : " << gpa << endl;
        }
      }
    }
    

  }
 
}






int main()
{
    string filename;

    cout << std::fixed;
    cout << std::setprecision(2);

    //
    // 1. Input the filename and then the grade data:
    //
    cin >> filename;
    // filename = "fall-2018.csv";

    College c = InputGradeData(filename);

    //]
    cout << "** College of " << c.Name << ", " << c.Semester << 
      " " << c.Year << " **" << endl;
    cout << "# of courses taught: " << c.NumCourses() << endl;
    cout << "# of students taught: " << c.NumStudents() << endl; 
  
    GradeStats s = GetGradeDistribution(c);
    cout << "grade distribution (A-F): " << s.PercentA << "%, ";
    cout << s.PercentB << "%, ";
    cout << s.PercentC << "%, ";
    cout << s.PercentD << "%, ";
    cout << s.PercentF << "%" << endl;
    
    int dfw;
    int n;
    
    cout << "DFW rate: " << GetDFWRate(c, dfw, n) << "%" << endl;
    
    cout << endl;
    cout << "Enter a command> ";
    //
  ]
    
    string user_input;
    string dept_name;
    string insPre; 
    double rate;
    cin >> user_input;
  
    while(user_input != "#"){
      
      if(user_input == "summary"){
         cout << "dept name, or all? ";
         cin >> dept_name;
         summary(c, dept_name);
      }
      else if(user_input == "search"){
        cout << "dept name, or all? ";
        cin >> dept_name;
        cout << "course # or instructor prefix? ";
        cin >> insPre;
        search(c, dept_name, insPre);
      }
      else if(user_input == "satisfactory"){
        cout << "dept name, or all? ";
        cin >> dept_name;
        satisfactory(c, dept_name);
      }
      else if(user_input == "dfw"){
        cout << "dept name, or all? ";
        cin >> dept_name;
        cout << "dfw threshold? ";
        cin >> rate;
        dfw_rate(c, dept_name, rate);
      }
      else if(user_input == "letterB"){
        cout << "dept name, or all? ";
        cin >> dept_name;
        cout << "letter B threshold? ";
        cin >> rate;
        letterB(c, dept_name, rate);
      }
      else if(user_input == "average"){
        cout << "dept name, or all? ";
        cin >> dept_name;
        average(c, dept_name);
      }
      else{
        cout << "**unknown command" << endl;
      }
      
      //cout << endl;
      cout << "Enter a command> ";
      cin >> user_input;
    }
    //cout << "hoya" << endl;
    

    //
    // done:
    //
    return 0;
}