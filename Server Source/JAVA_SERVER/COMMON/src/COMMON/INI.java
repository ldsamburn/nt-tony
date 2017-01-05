/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

//Description:  An object which can be used to access INI files


package COMMON;

import java.util.*;
import java.io.*;

public class INI
{

private final static int NONE=0;
private final static int SECTION=1;
private final static int ITEM=2;
private final static int MAX_INI_ENTRY=256;

public final static String SECTION_BEGIN_DELIMITER="[";
public final static String SECTION_END_DELIMITER="]";
public final static String TAG_VALUE_DELIMITER="=";
public final static String STRINGS_DELIMITER="~";
private final static String STRINGS_LABEL="Strings";

private Vector head_inst = new Vector();

private String errorMessage = null;

/**
 * Creates an _INI_Instance object to represent the INI file passed in and
 *  loads the object with all the data from the INI file, if it has not already
 *  been done.  The _INI_Instance object is added to the list of _INI_Instance
 *  objects "head_inst".
 */
private int LoadINIFile(String filename)
{
  try {
    FileReader   fp;
    BufferedReader br;
    String ini_entry;
    String ini_entry_ptr;
    int    state_ind;
    String section;
    _INI_Instance   cur_inst;

    state_ind = NONE;

    // Look for the _INI_Instance object that represents the INI filename
    //  passed in. If it is not found, add it to the list of _INI_Instances
    //  and load the file data into it. Else if it is found, just return here.
    cur_inst = FindINIInstance(filename);
    if (cur_inst == null)
      {
        cur_inst = AddINIInstance(filename);
      }
    else
      {
        return(0);
      }

    // Try to open the INI file for reading
    try {
      fp = new FileReader(filename);
    }
    catch (Exception exception) {
      // Just return a -1.  The file was not valid.
      RecordErrorMsg("LoadINIFile 2" + exception.toString());
      return(-2);
    }
    br = new BufferedReader(fp);
    if (!br.ready())
      {
        RecordErrorMsg("LoadINIFile 2: Problems reading file " + filename);
        return -3;
      }


    // Read each line from the file and add the data to the _INI_Instance object
    section = null;
    do {

      // Read a line from the INI file
      try {
        ini_entry_ptr = br.readLine();
      }
      catch (Exception exception) {
        RecordErrorMsg("LoadINIFile 2 " + exception.toString());
        br.close();
        return(-4);
      }
      // Figure out what type data is on this line
      switch(DetermineEntryType(ini_entry_ptr))	{
      
        case SECTION:       // This is a line that begins a section
          section = GetSectionNameOnly(ini_entry_ptr);
          AddINISectionLink(ini_entry_ptr, cur_inst);
          if (cur_inst.head_sect == null) {
            RecordErrorMsg("Error Adding Section Link");
            br.close();
            return(-5);
          }
          state_ind = SECTION;
          break;

        case ITEM:   // This is a line within a section
          if (cur_inst.head_sect == null) {
            break;
          }
          if (AddINIItemLink(section, ini_entry_ptr, cur_inst)==1) {
            RecordErrorMsg("Error Adding Item Link");
            br.close();
            return(-6);
          }
          state_ind = ITEM;
          break;

        default:
          {
            break;
          }
		  }
    } while   (ini_entry_ptr != null);

    // Close the INI file
    br.close();

    return(0);
  }
  catch (Exception exception) {
      RecordErrorMsg("LoadINIFile 3 " + exception.toString());
      return(-1);
  }

}


/**
 * Creates an _INI_Instance object to represent the INI string passed in and
 *  loads the object with all the data from the INI string, if it has not
 *  already  been done.  The _INI_Instance object is added to the list
 *  of _INI_Instance objects "head_inst".
 * The identifier should be a common filename so if the flush() method
 *  is called, the contents will end up in a valid file.
 */

public int LoadINIString( String  str_INI_contents,
                          String  identifier )
{
  try {
    String  ini_entry;
    String  ini_entry_ptr;
    int     state_ind;
    String  section;
    _INI_Instance   cur_inst;

    state_ind = NONE;

    // Look for the _INI_Instance object that represents the identifier
    //  passed in. If it is not found, add it to the list of _INI_Instances
    //  and load the input data into it. Else if it is found, just return here.
    cur_inst = FindINIInstance(identifier);
    if (cur_inst == null)
      cur_inst = AddINIInstance(identifier);

    section = null;

    BufferedReader  buff_reader;
    buff_reader = new BufferedReader(new StringReader(str_INI_contents));

    // Read each "line" from the string and add the data to the
    //  _INI_Instance object.  This is sort of an additive-overwrite feature.
    // This will overwrite any pre-existing profile
    //  string values that are already in the targeted INI entity (if it exists).
    ini_entry_ptr = buff_reader.readLine();  // prime the the temp variable..
    while (ini_entry_ptr != null)
      {
        // Figure out what type data is on this line
        switch(DetermineEntryType(ini_entry_ptr))
          {
            case SECTION:       // This is a line that begins a section
              {
                section = GetSectionNameOnly(ini_entry_ptr);
                AddINISectionLink(ini_entry_ptr, cur_inst);

                if (cur_inst.head_sect == null)
                  {
                    RecordErrorMsg("Error Adding Section Link");
                    return -2;
                  }
                state_ind = SECTION;
                break;
              }
            case ITEM:   // This is a line within a section
              {
                if (cur_inst.head_sect == null)
                  {
                    break;
                  }
                if (AddINIItemLink(section, ini_entry_ptr, cur_inst)==1)
                  {
                    RecordErrorMsg("Error Adding Item Link");
                    return -3;
                  }
                state_ind = ITEM;
                break;
              }

            default:
              break;
		      }

          // Read in the next line from the INI "file" string
        try
          {
            ini_entry_ptr = buff_reader.readLine();
          }
        catch (Exception exception)
          {
            RecordErrorMsg("LoadINIFile 2 " + exception.toString());
            return -1;
          }
      } // end of the reading loop

    return 0;
  }
  catch (Exception exception)
  {
    RecordErrorMsg("LoadINIFile final " + exception.toString());
    return(-4);
  }

}


/**
 * Each INI file is represented by an _INI_Instance object which stored in the
 *  "head_inst" Vector object. This method searches through the "head_inst"
 *  object for the instance object that matches the INI filename passed in.
 *  If the instance is found, it is returned. Otherwise, "null" is returned.
 */
private _INI_Instance FindINIInstance(String fn)
{
  try {
    _INI_Instance t = null;
    if (head_inst == null)
      return null;

    for (int index=0; index<head_inst.size(); index++) {
      t = (_INI_Instance)head_inst.elementAt(index);
      if (fn.equalsIgnoreCase(t.fn)) {
        return(t);
      }
  	}
    return(null);
  }
    catch (Exception exception) {
      RecordErrorMsg("FindINIInstance " + exception.toString());
    return(null);
  }

}

/**
 * Each INI file is represented by an _INI_Instance object which stored in the
 *  "head_inst" Vector object. This method creates and _INI_Instance object
 *  for the INI filename passed in, adds it to the "head_inst" list, and
 *  returns it to the calling method.
 */
private _INI_Instance AddINIInstance(String fn)
{
  try {
    _INI_Instance t = new _INI_Instance();

  	t.fn = fn;
    t.strings_sect_flag = 0;
    head_inst.addElement(t);
    return(t);
  }
    catch (Exception exception) {
      RecordErrorMsg("AddINIInstance " + exception.toString());
    return(null);
  }
}

private int DetermineEntryType(String entry)
{
  try {
    if (entry==null)
      return(NONE);

    if (entry.startsWith(SECTION_BEGIN_DELIMITER))
      return(SECTION);

    if (entry.indexOf(TAG_VALUE_DELIMITER)>=0)
      return(ITEM);

    return(NONE);
  }
    catch (Exception exception) {
      RecordErrorMsg("DetermineEntryType " + exception.toString());
    return(-1);
  }
}

/**
 * Creates an _INI_Section object and adds it to the list of INI sections
 *  (_INI_Instance.heads_sect) for the _INI_Instance object "cur_inst".
 */
private int AddINISectionLink(String        entry,
                              _INI_Instance cur_inst )
{
  try {
    // Create an _INI_Section object
    _INI_Section ts = new _INI_Section();

  	ts.section = GetSectionNameOnly(entry);

    if (cur_inst.head_sect == null)
      cur_inst.head_sect = new Vector();

    // Add the _INI_Section object to the _INI_Instance object member
    //  (head_sect) which contains a list of all the INI sections in this
    //  _INI_Instance 
    cur_inst.head_sect.addElement(ts);

    // Need to ask tony about this variable in the class
    if (ts.section.indexOf(STRINGS_LABEL)>=0)
      cur_inst.strings_sect_flag = 1;

  	return(0);
  }
    catch (Exception exception) {
      RecordErrorMsg("AddINISectionLink " + exception.toString());
    return(-1);
  }
}

/**
 * Takes a section title and an item ("entry") from the section from the INI
 *  file, and adds the item to the _INI_Section object in the _INI_Instance
 *  object.
 */
private int AddINIItemLink(String        section,
                           String        entry,
                           _INI_Instance cur_inst)
{
  try {
    _INI_Section  ts = null;
    _INI_Item  is = new _INI_Item();

    // Get the _INI_Section object from the _INI_Instance object that
    //  matches the section title passed in
    ts = FindINISection(section, cur_inst.head_sect);
    if (ts == null)
      {
        AddINISectionLink(section, cur_inst);
        ts = FindINISection(section, cur_inst.head_sect);
	  }

    // Initialize an _INI_Item object with the line from the INI file
  	 if (GetItemValues(entry, is) != 0) {
	   	RecordErrorMsg("INI File OUT OF Format!");
		   return(1);
  	  }

    // Add the new _INI_Item to the list of _INI_Items for this _INI_Section
    //  object
    if (ts.first_item == null)
      ts.first_item = new Vector();
    ts.first_item.addElement(is);

	  return(0);
  }
    catch (Exception exception) {
      RecordErrorMsg("AddINIItemLink " + exception);
      return(-1);
  }
}

private _INI_Section FindINISection(String lpszSection,
                                    Vector head_sect)
{
  try {
    _INI_Section   ts;
    String newSectionNameOnly;

    if (head_sect == null)
      return null;

    newSectionNameOnly = GetSectionNameOnly(lpszSection);

    ts = null;
    for (int index=0; index<head_sect.size(); index++) {
      ts = (_INI_Section)head_sect.elementAt(index);
      if (newSectionNameOnly.equalsIgnoreCase(ts.section))
        return(ts);
    }
    return(null);
  }
    catch (Exception exception) {
      RecordErrorMsg("FindINISection " + exception.toString());
    return(null);
  }

}

private int GetItemValues(String entry,
                          _INI_Item iniItem)
{
  try {
    int tagValueDelimiterIndex = entry.indexOf(TAG_VALUE_DELIMITER);

    if (tagValueDelimiterIndex < 0)
      return -1;

    iniItem.item = entry.substring(0,tagValueDelimiterIndex);
    iniItem.value = entry.substring(tagValueDelimiterIndex+1,entry.length());

    return 0;
  }
    catch (Exception exception) {
      RecordErrorMsg("FindINISection " + exception.toString());
    return(-1);
  }
}

public int WalkINIEntries(String fn)
{
  try {
    Vector ts;
    Vector is;

    _INI_Instance cur_inst;
    _INI_Item     newINIItem;
    _INI_Section  newSectionClass;

    if(LoadINIFile(fn) < 0)
      return(-1);

    cur_inst = FindINIInstance(fn);

    if (cur_inst == null)
      return(-1);

    if (cur_inst.head_sect == null)
      System.out.println("No Sections Found");
    else {
      ts = cur_inst.head_sect;
      for (int index=0; index<ts.size(); index++) {
        newSectionClass = (_INI_Section)(ts.elementAt(index));
        System.out.println("Section " + newSectionClass.section);

        is = newSectionClass.first_item;
        if (is == null)
          System.out.println("No Items to Walk");
        else {
          for (int index2=0; index2<is.size(); index2++) {
            newINIItem = (_INI_Item)is.elementAt(index2);
            System.out.println("     Tag="+newINIItem.item+"   Value="+newINIItem.value);
          }
        }
      }
	  }
    return(0);
  }
    catch (Exception exception) {
      RecordErrorMsg("WalkINIEntries " + exception.toString());
    return(-1);
  }
}

public int FlushINIFile(String fn)
{
  try {
    Vector ts;
    Vector is;
    _INI_Instance cur_inst;
    _INI_Section  newSectionClass;
    _INI_Item     newINIItem;

    FileWriter   fw;
    BufferedWriter bw;

    cur_inst = FindINIInstance(fn);
    if (cur_inst == null)
      {
        RecordErrorMsg("FlushINIFile - instantiate: No instance of " + fn
                       + " was found.");
        return(-1);
      }

    if (cur_inst.head_sect != null) {

      try {
        fw = new FileWriter(fn);
        bw = new BufferedWriter(fw);
      }
      catch (Exception exception) {
        RecordErrorMsg("FlushINIFile - instantiate " + exception.toString());
        return -2;
      }

      ts = cur_inst.head_sect;
      for (int index=0; index<ts.size(); index++) {
        newSectionClass = (_INI_Section)(ts.elementAt(index));
        try {
          bw.write(BuildSectionLabel(newSectionClass.section));
          bw.newLine();
        }
        catch (Exception exception) {
          RecordErrorMsg("FlushINIFile - bw.write(BuildSectionLabel) " +
                         exception.toString());
          return -3;
        }

        is = newSectionClass.first_item;
        if (is != null) {
          for (int index2=0; index2<is.size(); index2++) {
            newINIItem = (_INI_Item)is.elementAt(index2);
            try {
              bw.write(BuildProfileString(newINIItem.item,newINIItem.value));
              bw.newLine();
            }
            catch (Exception exception) {
              RecordErrorMsg("FlushINIFile - bw.write(newItem) " +
                             exception.toString());
              return -4;
            }
          }
          try {
            bw.newLine();
          }
          catch (Exception exception) {
            RecordErrorMsg("FlushINIFile - bw.newLine " + exception.toString());
            return -5;
          }
        }
      }
      try {
        bw.flush();
        bw.close();
      }
      catch (Exception exception) {
        RecordErrorMsg("FlushINIFile - bw.flush " + exception.toString());
      }
  	}
    return(0);
  }
    catch (Exception exception) {
      RecordErrorMsg("WalkINIEntries " + exception.toString());
      return(-6);
  }
}

private int DeleteINILinks(String fn)
{
  try {
    Vector ts;
    Vector is;
    _INI_Instance cur_inst;
    _INI_Section  newSectionClass;

    cur_inst = FindINIInstance(fn);
    if (cur_inst == null)
      return (0);

    if (cur_inst.head_sect == null) {
      RecordErrorMsg("No Sections Found");
      return 0;
    } else {
      ts = cur_inst.head_sect;
      for (int index=0; index<ts.size(); index++) {
        newSectionClass = (_INI_Section)(ts.elementAt(index));
        is = newSectionClass.first_item;
        if (is != null)
          is.removeAllElements();
      }
      ts.removeAllElements();
      cur_inst=null;
  	}
    return(0);
  }
    catch (Exception exception) {
      RecordErrorMsg("DeleteINILinks " + exception.toString());
    return(-1);
  }
}

private _INI_Item FindINIItem(Vector lpsection,
                              String lpszItem )
{
  try {
    _INI_Item   is;

    if (lpsection == null)
      return null;

    for (int index=0; index<lpsection.size(); index++) {
      is = (_INI_Item)lpsection.elementAt(index);
      if (lpszItem.equalsIgnoreCase(is.item)) {
        return(is);
      }
    }
    return null;
  }
  catch (Exception exception) {
    RecordErrorMsg("FindINIItem " + exception.toString());
    return null;
  }
}

private int DelINIItem(_INI_Section lpsection,
                       String       lpszItem )
{
  try {
    Vector    is;
    _INI_Item is1;

    is = lpsection.first_item;
    if (is != null) {
      for (int index=0; index<is.size(); index++) {
        is1 = (_INI_Item)is.elementAt(index);
        if (lpszItem.equalsIgnoreCase(is1.item)) {
          is.removeElementAt(index);
          return 0;
        }
      }
    }
    return -1;
  }
  catch (Exception exception) {
    RecordErrorMsg("DelINIItem " + exception.toString());
    return -1;
  }
}

private String GetINISection(String        lpszSection,
                             _INI_Instance cur_inst )
{
  try {
    _INI_Section ts;
    _INI_Item    newINIItem;

    Vector       is;

    String       lpszPtr = null;
    ts = FindINISection(lpszSection, cur_inst.head_sect);
    if (ts == null)
      return (lpszPtr);
    is = ts.first_item;
    if (is != null) {
      for (int index2=0; index2<is.size(); index2++) {
        if (lpszPtr == null)
          lpszPtr = new String();
        newINIItem = (_INI_Item)is.elementAt(index2);
        lpszPtr += BuildProfileString(newINIItem.item,newINIItem.value) + STRINGS_DELIMITER;
      }
    }
    return(lpszPtr);
  }
  catch (Exception exception) {
    RecordErrorMsg("GetINISection " + exception.toString());
    return null;
  }
}

private String GetINISectionNames(_INI_Instance cur_inst)
{
  try {
    Vector       ts;

    _INI_Section ts1;

    String       lpszPtr = null;

    ts = cur_inst.head_sect;
    for (int index=0; index<ts.size(); index++) {
      if (lpszPtr == null)
        lpszPtr = new String();
      ts1 = (_INI_Section)(ts.elementAt(index));
      lpszPtr += ts1.section + STRINGS_DELIMITER;
    }
    return lpszPtr;
  }
  catch (Exception exception) {
    RecordErrorMsg("GetINISectionNames " + exception.toString());
    return null;
  }
}

public String GetProfileSection(String  lpszSection,
                                String  fn )
{
  try {
    String        lpszPtr = null;
    _INI_Instance cur_inst;

  	if (LoadINIFile(fn) < 0) {
	  	return(lpszPtr);
    }

  	if (lpszSection == null)
	  	return (lpszPtr);

  	cur_inst=FindINIInstance(fn);
	  if (cur_inst == null) {
		  RecordErrorMsg("Unable to find INI Instance for "+fn);
  		return(lpszPtr);
	  }

    lpszPtr = GetINISection(lpszSection,cur_inst);
    return lpszPtr;
  }
  catch (Exception exception) {
    RecordErrorMsg("GetProfileSection " + exception.toString());
    return null;
  }
}

public String GetProfileSectionNames(String fn )
{
  try {
    String        lpszPtr = null;
    _INI_Instance cur_inst;

  	if (LoadINIFile(fn) < 0)
	  	return(lpszPtr);

  	cur_inst=FindINIInstance(fn);
	  if (cur_inst == null) {
		  RecordErrorMsg("Unable to find INI Instance for "+fn);
  		return(lpszPtr);
	  }
    lpszPtr = GetINISectionNames(cur_inst);
    return lpszPtr;
  }
  catch (Exception exception) {
    RecordErrorMsg("GetProfileSectionNames " + exception.toString());
    return null;
  }
}

public String GetSectionValues(String  lpszSection,
                        String  fn)
{
  try {
    String        lpszPtr = null;
    _INI_Instance cur_inst;

  	if (LoadINIFile(fn) < 0)
	  	return(lpszPtr);

  	if (lpszSection == null)
	  	return (lpszPtr);

  	cur_inst=FindINIInstance(fn);
	  if (cur_inst == null) {
		  RecordErrorMsg("Unable to find INI Instance for "+fn);
  		return(lpszPtr);
	  }
   lpszPtr = GetINISection(lpszSection,cur_inst);
   return lpszPtr;
  }
  catch (Exception exception) {
    RecordErrorMsg("GetSectionValues " + exception.toString());
    return null;
  }
}

public int GetProfileInt(String lpszSection,
                         String lpszKey,
                         int    dwDefault,
                         String fn )
{
  try {
    String        lpszPtr = null;
    _INI_Instance cur_inst;
    _INI_Section  ts;
    _INI_Item is;
    Integer returnInt;
    char[] returnValue;


  	if (LoadINIFile(fn) < 0)
	  	return(dwDefault);

  	if (lpszSection == null)
	  	return (dwDefault);

  	if (lpszKey == null)
	  	return (dwDefault);

  	cur_inst=FindINIInstance(fn);
	  if (cur_inst == null) {
		  RecordErrorMsg("Unable to find INI Instance for " + fn);
  		return(dwDefault);
	  }
    ts = FindINISection(lpszSection, cur_inst.head_sect);
    if (ts == null) {
  		return(dwDefault);
    }
    is = FindINIItem(ts.first_item,lpszKey);
    if (is == null)
  		return(dwDefault);

    if (is.value.length() == 0)
      return(dwDefault);

    returnValue = new char[is.value.length()];
    returnValue = is.value.toCharArray();
    for (int i = 0; i < is.value.length(); i++)
    {
      if (!Character.isDigit(returnValue[i]))
        return(dwDefault);
  	}
    returnInt = new Integer(is.value);

    if (returnInt == null)
  		return(dwDefault);

    return (returnInt.intValue());

  }
  catch (Exception exception) {
    RecordErrorMsg("GetProfileInt " + exception.toString());
    return (dwDefault);
  }
}

public String GetProfileString(String lpszSection,
                               String lpszKey,
                               String lpszDefault,
                               String fn )

{
  try {
    String        lpszPtr = null;
    _INI_Instance cur_inst;
    _INI_Section  ts;
    _INI_Item is;

  	if (LoadINIFile(fn) < 0)
	  	return(lpszDefault);

  	if (lpszSection == null)
      {
        RecordErrorMsg("Null section provided.");
	  	  return (lpszDefault);
      }

  	if (lpszKey == null)
      {
        RecordErrorMsg("Null key provided.");
	  	  return (lpszDefault);
      }

  	cur_inst=FindINIInstance(fn);
	  if (cur_inst == null)
      {
		    RecordErrorMsg("Unable to find INI Instance for " + fn);
    		return(lpszDefault);
	    }
    ts = FindINISection(lpszSection, cur_inst.head_sect);
    if (ts == null)
      {
        RecordErrorMsg("Unable to find INI section " + lpszSection);
    		return(lpszDefault);
      }

    is = FindINIItem(ts.first_item, lpszKey);
    if (is == null)
      {
        RecordErrorMsg("Key does not exist: " + lpszKey);
  		  return(lpszDefault);
      }

    return (is.value);

  }
  catch (Exception exception) {
    RecordErrorMsg("GetProfileString " + exception.toString());
    return (lpszDefault);
  }
}

/**
 *  This method completely overwrites profile section with the given set of
 * tag-value pairs.
 **/
public int WriteProfileSection(String lpszSection,
                               String[] lpszKeyAndValues,
                               String fn )
{
  try {
    _INI_Section  ts;
    _INI_Instance cur_inst;
  	LoadINIFile(fn);

  	if (lpszSection == null)
      return (-1);

  	cur_inst=FindINIInstance(fn);
	  if (cur_inst == null) {
		  RecordErrorMsg("Unable to find INI Instance for "+fn);
  		return(-1);
	  }
    ts = FindINISection(lpszSection, cur_inst.head_sect);
    if (ts == null) {
      lpszSection = BuildSectionLabel(lpszSection);
      AddINISectionLink(lpszSection, cur_inst);
      ts = FindINISection(lpszSection, cur_inst.head_sect);
    }
    if (ts.first_item != null)
      ts.first_item.removeAllElements();
    for (int index=0; index<lpszKeyAndValues.length; index++) {
      if (AddINIItemLink(lpszSection, lpszKeyAndValues[index], cur_inst)==1) {
        RecordErrorMsg("Error Adding Item Link");
        return(-1);
      }
    }

    return 0;
  }
  catch (Exception exception) {
    RecordErrorMsg("WriteProfileSection " + exception.toString());
    return (-1);
  }
}


/*
 * This method can handle the string given back from a
 * GetProfileSection() result as its input.  This method does not erase profile
 * strings that do not exist in the provided key=value pairs (like a
 * WriteProfileSection call would normally do) but just adds in
 * non-pre-existing pairs and overwrites pre-existing pairs.
 */
public int UpdateProfileSection(String  section_name,
                                String  input_string,
                                String  ini_filename )
{
  final String  method_name = "UpdateProfileSection(): ";
  try {
    _INI_Section  ts;
    _INI_Instance cur_inst;
    _INI_Item     is;
    String        key;
    String        value;
    String        key_value_pair;
    int           delimiter_index;

  	LoadINIFile(ini_filename);
  	if (section_name == null)
      return (-2);

  	cur_inst = FindINIInstance(ini_filename);
	  if (cur_inst == null)
      {
		    RecordErrorMsg(method_name + "Unable to find INI Instance for "
                       + ini_filename);
    		return(-3);
	    }
    ts = FindINISection(section_name, cur_inst.head_sect);
    if (ts == null)
      {
        section_name = BuildSectionLabel(section_name);
        AddINISectionLink(section_name, cur_inst);
        ts = FindINISection(section_name, cur_inst.head_sect);
      }

    // We expect each key=value pair to be separated by newlines...
    StringTokenizer tokenizer = new StringTokenizer(input_string,
                                                    STRINGS_DELIMITER);
    while (tokenizer.hasMoreElements() == true)
      {
        key_value_pair = tokenizer.nextToken();
        delimiter_index = key_value_pair.indexOf(TAG_VALUE_DELIMITER);
        key = key_value_pair.substring(0, delimiter_index);
        is = FindINIItem(ts.first_item, key);
        if (is != null)
          {
            if (UpdateProfileString(
                      key_value_pair.substring(delimiter_index + 1), is) != 0)
              return(-4);
            continue;
          }
        // else we assume this entry is brand-new.
        if (AddINIItemLink(section_name, key_value_pair, cur_inst )== 1)
          {
            RecordErrorMsg(method_name + "Error Adding Item Link");
            return(-5);
          }
      }

    return 0;
  }
  catch (Exception exception) {
    RecordErrorMsg(method_name + exception.toString());
    return (-1);
  }
}


public int  WriteProfileString(String  lpszSection,
                               String  lpszKey,
                               String  lpszString,
                               String  fn )

{
  try {
    _INI_Section  ts;
    _INI_Item     is;
    _INI_Instance cur_inst;

  	LoadINIFile(fn);

  	if (lpszSection == null)
      return (-1);

  	cur_inst=FindINIInstance(fn);
	  if (cur_inst == null) {
		  RecordErrorMsg("Unable to find INI Instance for "+fn);
  		return(-1);
	  }
    ts = FindINISection(lpszSection, cur_inst.head_sect);
    if (ts == null) {
      lpszSection = BuildSectionLabel(lpszSection);
      AddINISectionLink(lpszSection, cur_inst);
      ts = FindINISection(lpszSection, cur_inst.head_sect);
    }

    is = FindINIItem(ts.first_item, lpszKey);

    if (is != null) {
      return (UpdateProfileString(lpszString,is));
    }

    String lineString = BuildProfileString(lpszKey, lpszString);
    if (AddINIItemLink(lpszSection, lineString, cur_inst)==1) {
      RecordErrorMsg("Error Adding Item Link");
      return(-1);
    }

    return 0;
  }
  catch (Exception exception) {
    RecordErrorMsg("WriteProfileString " + exception);
    return (-1);
  }
}

public int RemoveProfileString(String lpszSection,
                               String lpszKey,
                               String fn )
{
  try {
    _INI_Section  ts;
    _INI_Item     is;
    _INI_Instance cur_inst;

  	if (LoadINIFile(fn) < 0)
      return (-1);

  	if (lpszSection == null)
      return (-1);
  	if (lpszKey == null)
      return (-1);

  	cur_inst=FindINIInstance(fn);
	  if (cur_inst == null) {
		  RecordErrorMsg("Unable to find INI Instance for "+fn);
  		return(-1);
	  }
    ts = FindINISection(lpszSection, cur_inst.head_sect);
    if (ts != null) {
      DelINIItem(ts, lpszKey);
    }

    return 0;
  }
  catch (Exception exception) {
    RecordErrorMsg("RemoveProfileString " + exception.toString());
    return (-1);
  }

}

/**
 * Removes the INI section "lpszSection" from the given INI file
 */
public int RemoveProfileSection(String lpszSection, String fn)
{
    _INI_Section section;
    Vector is;
    _INI_Instance cur_inst;

   try {
      // Check to make sure a section name was passed in
      if (lpszSection == null)
         return (-1);

      // Load the INI file into memory if it has not already been done
      if (LoadINIFile(fn) < 0)
         return (-1);

      // Find the _INI_Instance object that represents the INI filename
      //  passed in
      cur_inst = FindINIInstance(fn);
      if (cur_inst == null)
         return (0);

      // Check to make sure the loaded INI file even has any data
      if (cur_inst.head_sect == null) {
         RecordErrorMsg("No Sections Found");
         return 0;
      }

      // Loop through all the sections in this _INI_Instance object
      for (int i = 0; i < (cur_inst.head_sect).size(); i++) {
         section = (_INI_Section)(cur_inst.head_sect).elementAt(i);

         // Check to see if the current section title equals the name of
         //  the section we're looking for
         if (section.section.equals(lpszSection) == true) {

            // Remove the section from the list (_INI_Instance.head_sect)
            (cur_inst.head_sect).removeElementAt(i);
            break;
         }
      }

      // If the last section was removed, set the section list equal to null
      if ((cur_inst.head_sect).size() == 0)
         cur_inst.head_sect = null;

      return(0);
   }
   catch (Exception exception) {
      RecordErrorMsg("RemoveProfileSection " + exception.toString());
      return (-1);
   }
}

public String GetAllLoadedFiles()
{
  try {
    String lpszPtr = null;
    _INI_Instance t = null;
    for (int index=0; index<head_inst.size(); index++) {
      t = (_INI_Instance)head_inst.elementAt(index);
      if (lpszPtr == null)
        lpszPtr = new String();
      lpszPtr += t.fn + STRINGS_DELIMITER;
    }
    return (lpszPtr);
  }
  catch (Exception exception) {
    RecordErrorMsg("GetAllLoadedFiles " + exception.toString());
    return (null);
  }

}

private String BuildSectionLabel(String section_name)
{
  try {
    String lpszPtr = null;
    if (section_name != null)
      lpszPtr = SECTION_BEGIN_DELIMITER + section_name + SECTION_END_DELIMITER;

    return(lpszPtr);
  }
  catch (Exception exception) {
    RecordErrorMsg("BuildSectionLabel " + exception.toString());
    return (null);
  }
}

private String GetSectionNameOnly(String section_name)
{
  try {
    String newSectionName = null;
    int sectionBeginDelimiterIndex = section_name.indexOf(SECTION_BEGIN_DELIMITER)+1;
    int sectionEndDelimiterIndex = section_name.indexOf(SECTION_END_DELIMITER);
    if (sectionBeginDelimiterIndex < 0)
        sectionBeginDelimiterIndex = 0;
    if (sectionEndDelimiterIndex < 0)
        sectionEndDelimiterIndex = section_name.length();
    newSectionName = section_name.substring(sectionBeginDelimiterIndex,sectionEndDelimiterIndex);
    if (newSectionName == null)
      newSectionName =  section_name;

    return (newSectionName);
  }
  catch (Exception exception) {
    RecordErrorMsg("BuildSectionLabel " + exception.toString());
    return (null);
  }
}


private String BuildProfileString(String key_str,
                                  String value_str )
{
  try {
    String output_str = null;
    if (key_str==null)
      return output_str;
    if (value_str==null)
      return output_str;

    output_str=key_str+TAG_VALUE_DELIMITER+value_str;
    return(output_str);
  }
  catch (Exception exception) {
    RecordErrorMsg("BuildProfileString " + exception.toString());
    return (null);
  }
}

private int UpdateProfileString(String  lpszString,
                                _INI_Item iniItem)
{
  try {
    iniItem.value = lpszString;
    return(0);
  }
  catch (Exception exception) {
    RecordErrorMsg("UpdateProfileString " + exception.toString());
    return (-1);
  }
}

/**
 * Records the given message in a buffer
 */
private void RecordErrorMsg(String msg)
{
   errorMessage = msg;
}

/**
 * Gets the last recorded message
 */
public String GetLastErrorMsg()
{
   return errorMessage;
}

/**
 * Represents a single item in an INI section
 */
private class _INI_Item {
	public String item;
	public String value;
}

/**
 * Represents an INI section
 */
private class _INI_Section  {
	public String section;
	public Vector  first_item;   // Set of _INI_Item objects
}

/**
 * Represents an INI file
 */
private class _INI_Instance {
	public String fn;
	public Vector  head_sect;    // Set of _INI_Section objects
   public int strings_sect_flag;
}

}
