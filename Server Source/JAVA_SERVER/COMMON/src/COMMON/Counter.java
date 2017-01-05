/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

//Description:  A simple counter object that can be initialized to a certain
//               value and then incremented and decremented. The main feature
//               of this object is that its members are synchronized (i.e. in
//               a multithreaded environment, only one thread can operate on an
//               instance of the class at a time).


package COMMON;


public class Counter
{
   private long value;  // Holds the value of the "counter"

   public Counter(long init)
   {
      value = init;
   }

   // Increments the counter value by one
   public synchronized void increment()
   {
      value++;
   }

   // Decrements the counter value by one
   public synchronized void decrement()
   {
      value--;
   }

   // Returns the current value of the counter
   public synchronized long getValue()
   {
      return value;
   }
}