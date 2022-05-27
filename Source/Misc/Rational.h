#pragma once 
#include "JuceHeader.h"

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
        const int maxDenominator = 1000;
        const int sign = x < 0 ? -1 : 1;
        x = fabs(x);
        const int integerPart = int(x);
        x -= integerPart;
        if(x <= epsilon)
        {
            numerator   = sign * integerPart;
            denominator = 1;
        }
        else
        {
            // approximation using the Farey sequence

            Rational q1(0,1);
            Rational q2(1,1);
            Rational med;
            double r;
            bool done = false;
            
            while(!done)
            {
                med.numerator = q1.numerator + q2.numerator;
                med.denominator = q1.denominator + q2.denominator;
                
                if(med.denominator > maxDenominator)
                {
                    numerator = sign * (q1.numerator + integerPart * q1.denominator);
                    denominator = q1.denominator;
                    done = true;
                }

                r = double(med.numerator) / med.denominator;

                if(std::abs(r - x) < epsilon)
                {
                    numerator = sign * (med.numerator + integerPart * med.denominator);
                    denominator = med.denominator;
                    done = true;
                }
                if(x < r)
                    q2 = med;
                else
                    q1 = med;
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
        int sign = toFloat() < 0 ? -1 : 1;

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
