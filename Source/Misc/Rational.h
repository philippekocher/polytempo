/* ==============================================================================
 
 This file is part of the POLYTEMPO software package.
 Copyright (c) 2016 - Zurich University of the Arts,
 ICST Institute for Computer Music and Sound Technology
 http://www.icst.net
 
 Author: Philippe Kocher
 
 POLYTEMPO is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 POLYTEMPO is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this software. If not, see <http://www.gnu.org/licenses/>.
 
 ============================================================================== */

#ifndef __Rational__
#define __Rational__

#include "../JuceLibraryCode/JuceHeader.h"

class Rational
{
public:
    Rational()
    {
        numerator   = 0;
        denominator = 1;
    }
    
    Rational(const String string)
    {
        StringArray tokens;
        tokens.addTokens(string, "/", "");
        tokens.removeEmptyStrings();
        tokens.trim();
        
        numerator   = tokens[0].getIntValue();
        denominator = tokens[1].getIntValue();
        
        if(denominator == 0) denominator = 1;
    }
    
    Rational(const int number)
    {
        numerator   = number;
        denominator = 1;
    }
    
    Rational(const int n, const int d)
    {
        numerator   = n;
        denominator = d;

        if(denominator == 0) denominator = 1;
        
        reduce();
    }
    
    Rational(float x)
    {
        const double epsilon = 1.e-6;
        const int sign = x < 0 ? -1 : 1;
        x = fabs(x);
        const int integerPart = int(x);
        x -= integerPart;                 // x enthält jetzt den Nachkommaanteil
        if(x <= epsilon)                  // x == Ganzzahl abfangen
        {
            numerator   = sign * integerPart;
            denominator = 1;
        }
        else
        {
            // -- Verfahren von Farey

            Rational q1("0/1");
            Rational q2("1/1");
            bool done = false;
            
            while(!done)
            {
                Rational med(q1.numerator + q2.numerator, q1.denominator + q2.denominator); // Mediante bestimmen
                const double r = double(med.numerator) / med.denominator;
                if(std::abs(r - x) < epsilon)
                {
                    numerator = sign * (med.numerator + integerPart * med.denominator);
                    denominator = med.denominator;
                    done = true;
                }
                if(x < r)       // ist x < als die Mediante
                    q2 = med;   // dann ist sie die obere Grenze
                else
                    q1 = med;   // und ansonsten die untere Grenze
            }
        }
    }
    
    
    String toString()
    {
        if(denominator == 1) return String(numerator);
        return String(numerator)+"/"+String(denominator);
    }
    
    float  toFloat()  { return float(numerator)/denominator; }
    
    Rational& operator= (const Rational& other)
    {
        numerator   = other.numerator;
        denominator = other.denominator;
        
        return *this;
    }
    
    int getNumerator() { return numerator; }
    int getDenominator() { return denominator; }
    
    //---------------------------------------------------
    Rational& operator= (int number)
    {
        numerator   = number;
        denominator = 1;
        
        return *this;
    }
    
    Rational& operator+ (const Rational& other)
    {
        int n = numerator   * other.denominator + denominator * other.numerator;
        int d = denominator * other.denominator;
        
        Rational *result = new Rational(n,d);
        return *result;
    }

    Rational& operator+ (const int number)
    {
        int n = numerator + number*denominator;
        
        Rational *result = new Rational(n,denominator);
        return *result;
    }
    
    float operator+ (const float number)
    {
        return toFloat() + number;
    }
    
    Rational& operator+= (Rational& other)
    {
        *this = *this + other;
        return *this;
    }
    
    Rational& operator- (const Rational& other)
    {
        int n = numerator   * other.denominator - denominator * other.numerator;
        int d = denominator * other.denominator;
        
        Rational *result = new Rational(n,d);
        return *result;
    }
    
    float operator- (const float number)
    {
        return toFloat() + number;
    }
    
    Rational& operator* (const Rational& other)
    {
        int n = numerator * other.numerator;
        int d = denominator * other.denominator;
        
        Rational *result = new Rational(n,d);
        return *result;
    }
    
    Rational& operator* (const int number)
    {
        int n = numerator * number;
        
        Rational *result = new Rational(n,denominator);
        return *result;
    }

    float operator* (float number)
    {
        return toFloat() * number;
    }


    //---------------------------------------------------
    bool operator== (const Rational& other) const noexcept    { return compare(other) == 0; }
    bool operator!= (const Rational& other) const noexcept    { return compare(other) != 0; }
    bool operator<  (const Rational& other) const noexcept    { return compare(other) <  0; }
    bool operator<= (const Rational& other) const noexcept    { return compare(other) <= 0; }
    bool operator>  (const Rational& other) const noexcept    { return compare(other) >  0; }
    bool operator>= (const Rational& other) const noexcept    { return compare(other) >= 0; }
    
    bool operator== (const int other) const noexcept    { return compare(Rational(other)) == 0; }
    bool operator!= (const int other) const noexcept    { return compare(Rational(other)) != 0; }
    bool operator<  (const int other) const noexcept    { return compare(Rational(other)) <  0; }
    bool operator<= (const int other) const noexcept    { return compare(Rational(other)) <= 0; }
    bool operator>  (const int other) const noexcept    { return compare(Rational(other)) >  0; }
    bool operator>= (const int other) const noexcept    { return compare(Rational(other)) >= 0; }
    
private:
    int numerator;
    int denominator;
    
    int compare (const Rational& other) const noexcept
    {
        if(numerator*other.denominator > other.numerator*denominator) return 1;
        if(numerator*other.denominator < other.numerator*denominator) return -1;
        return 0;
    }
    
    void reduce()
    {
        int a, b, rem;
        int num = abs(numerator);
        int den = abs(denominator);
        int sign = numerator * denominator < 0 ? -1 : 1;

        if(num > den)
        {
            a = num;
            b = den;
        }
        else
        {
            a = den;
            b = num;
        }
        
        while (b != 0)
        {
            rem = a % b;
            a = b;
            b = rem;
        }
        
        numerator = sign * num / a;
        denominator = den / a;
    }
};



#endif  // __Rational__
