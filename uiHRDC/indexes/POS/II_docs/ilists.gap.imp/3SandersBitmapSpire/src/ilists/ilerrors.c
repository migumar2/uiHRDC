
/*////////////////
//Error handling//
////////////////*/

/*
char *error_il(int e){
  switch(e) {
    case 0:  return "No error"; break;
    case 1:  return "Out of memory"; break;
	case 30: return "Error reading source posting lists";break;
	case 31: return "Empty Source list or incomplete !!";break;
	
	case 40: return "default function for Intersect_2-op not initialized";break;
	case 41: return "default function for Intersect_N-op not initialized";break;
	case 42: return "default function for fsearch-op not initialized";break;
		
    case 2:  return "The text must end with a \\0"; break;
    case 5:  return "You can't free the text if you don't copy it"; break;
    case 20: return "Cannot create files"; break;
    case 21: return "Error writing the index"; break;
    case 22: return "Error writing the index"; break;
    case 23: return "Cannot open index; break";
    case 24: return "Cannot open text; break";
    case 25: return "Error reading the index"; break;
    case 26: return "Error reading the index"; break;
    case 27: return "Error reading the text"; break;
    case 28: return "Error reading the text"; break;
    case 99: return "Not implemented"; break;
    default: return "Unknown error";
  }
}

*/


char *error_il(int e){
	static char err[100];
  switch(e) {
    case 0:  strcpy(err,"No error"); break;
    case 1:  strcpy(err, "Out of memory"); break;
	case 30: strcpy(err, "Error reading source posting lists");break;
	case 31: strcpy(err, "Empty Source list or incomplete !!");break;
	
	case 40: strcpy(err, "default function for Intersect_2-op not initialized");break;
	case 41: strcpy(err, "default function for Intersect_N-op not initialized");break;
	case 42: strcpy(err, "default function for fsearch-op not initialized");break;
		
    case 2:  strcpy(err, "The text must end with a \\0"); break;
    case 5:  strcpy(err, "You can't free the text if you don't copy it"); break;
    case 20: strcpy(err, "Cannot create files"); break;
    case 21: strcpy(err, "Error writing the index"); break;
    case 22: strcpy(err, "Error writing the index"); break;
    case 23: strcpy(err, "Cannot open index; break");
    case 24: strcpy(err, "Cannot open text; break");
    case 25: strcpy(err, "Error reading the index"); break;
    case 26: strcpy(err, "Error reading the index"); break;
    case 27: strcpy(err, "Error reading the text"); break;
    case 28: strcpy(err, "Error reading the text"); break;
    case 99: strcpy(err, "Not implemented"); break;
    default: strcpy(err, "Unknown error");
  }
  return err;
}
