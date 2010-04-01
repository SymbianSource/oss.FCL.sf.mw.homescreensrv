<?php
function add_file_to_list($n,$nn){
  global $names,$nNames,$i;         
  if(!in_array($n,$names)){
    $names[$i]=$n;
    $nNames[$i]=$nn;
    $i++;
  }       
}
function add_file_to_list2($n,$nn){
  global $tnames,$tnNames,$u;         
  if(!in_array($n,$tnames)){
    $tnames[$u]=$n;
    $tnNames[$u]=$nn;
    $u++;
  }        
}
function renameFiles($path,$prefix,array $ext){
  $dr = opendir($path); 
  $names = array();
  $nNames = array();
  while (($file = readdir($dr)) !== false) { 
    if($file != "." && $file != "..") {
      if(is_dir($path.'/'.$file)){
        renameFiles($path.'/'.$file,$prefix,$ext);
      }else{
        if(in_array(strtolower(substr($file,strpos($file,'.')+1)),$ext)){
          if(strpos($file,$prefix)===false){                                                  
            rename($path.'/'.$file,$path.'/'.$prefix.$file);
            //print $file.'=>'.$prefix.$file.'<br />';
            if(strtolower(substr($file,strpos($file,'.')+1))=='rsc'){
              add_file_to_list2($file,$prefix.$file);
            }
            add_file_to_list($file,$prefix.$file);
          }
        }
      }
    } 
  } 
  closedir($dr);
  //print_r($names);
  //print_r($nNames);
}

function replaceFileNameIn($path,$name,$nName,$ext){
  $dh = opendir($path); 
  while (($file = readdir($dh)) !== false) { 
    if($file != "." && $file != "..") { 
      if(is_dir($path.'/'.$file)){
        replaceFileNameIn($path.'/'.$file,$name,$nName,$ext); 
      }else{
        if(!in_array(strtolower(substr($file,strpos($file,'.')+1)),$ext)){
          $c = file_get_contents($path.'/'.$file);
          foreach($name as $k=>$v){
            $c = preg_replace('^("|<)('.$name[$k].')^','$1'.$nName[$k].'$3',$c);//("|>)
          }
          //print '<i>'.$file.'</i><br><pre>'.$c.'</pre>';
          file_put_contents($path.'/'.$file,$c);
        }
      }
    } 
  } 
  closedir($dh); 
}

function replaceFileNames2In($path,$name,$nName,$ext,$prefix){
  Global $prefix; 
  $dh = opendir($path); 
  while (($file = readdir($dh)) !== false) { 
    if($file != "." && $file != "..") { 
      if(is_dir($path.'/'.$file)){
        replaceFileNames2In($path.'/'.$file,$name,$nName,$ext,$prefix); 
      }else{
        if(in_array(strtolower(substr($file,strpos($file,'.')+1)),$ext)){
          $c = file_get_contents($path.'/'.$file);
          
          $t = preg_match_all('^TARGET\x20+((.*))\x0A^',$c,$matches);
          //$c = preg_replace('^(TARGET\x20+)^  ','$1'.$prefix,$c);//add prefix to target name
          
          if($t>0){
            //$matches[0]=null;
            foreach($matches[1] as $k=>$v){
              if(preg_match('^'.$prefix.'^',$v)==0){
                if(strtolower(substr($v,strpos($v,'.')+1))=='dll'){
                  $a = str_replace('.dll','.lib',$v);
                  add_file_to_list2($a,$prefix.$a);
                }                                   
                add_file_to_list2($v,$prefix.$v);
              }
            }         
          }
                    
          foreach($name as $k=>$v){
            $c =  preg_replace('^(\x2F|\x28|\x20|\x09)('.$name[$k].')^','$1'.$nName[$k].'$3',$c);//(\x20|\x29|\x0A|\x09)
          }
          //print '<i>'.$file.'</i><br><pre>'.$c.'</pre>';
          file_put_contents($path.'/'.$file,$c);
        }
      }
    } 
  } 
  closedir($dh); 
}
function replaceFileNames3In($path,$name,$nName,$ext){
  $dh = opendir($path); 
  while (($file = readdir($dh)) !== false) { 
    if($file != "." && $file != "..") { 
      if(is_dir($path.'/'.$file)){
        replaceFileNames3In($path.'/'.$file,$name,$nName,$ext); 
      }else{
        //if(in_array(strtolower(substr($file,strpos($file,'.')+1)),$ext)){
          $c = file_get_contents($path.'/'.$file);
          //foreach($name as $k=>$v){
             //$c = preg_replace('^(\x2F|\x28|\x20|\x09)('.$name.')^','$1'.$nName,$c);
             $c = str_replace($name,$nName,$c);
          //}
          //print '<i>'.$file.'</i><br><pre>'.$c.'</pre>';
          file_put_contents($path.'/'.$file,$c);
        //}
      }
    } 
  } 
  closedir($dh); 
}

function display_files($path){
  $dh = opendir($path); 
  while (($file = readdir($dh)) !== false) { 
    if($file != "." && $file != "..") { 
      if(is_dir($path.'/'.$file)){
        display_files($path.'/'.$file); 
      }else{
        $c = file_get_contents($path.'/'.$file);
        print '<i>'.$file.'</i><br><pre>'.$c.'</pre>';
      }
    } 
  } 
}

//***** Used for storing edited files data ****************************
$names= array();  //original names
$nNames=array();  //edited names
$i=0;             //number of edited files

$tnames= array();  //original target names
$tnNames=array();  //edited target names
$u=0;             //number of edited files

//*********************************************************************
$root = 'sfr'; //source root
chdir($root);  //change current working directory
//*********************************************************************
$folder_to_check_for_renamed_files = 'homescreensrv/hs_app_idlefw';
$folder_to_rename = 'homescreensrv/hs_app_idlefw/';
$prefix = 'hs_app_';
$ext = array('h','cpp','inl','rss','loc','def','rh','hrh','lib','iby'); //extensions of the files that needs to be renamed.
$ext2 = array('inf','mmp'); //extensions for which method 2 is used to search and replace filenames in certain file.


/***************************************************************************************
Renames all the files in specified folder if the extension is found in array $ext.
Edited files original names are stored in $names array, modified names in $nNames array.
*******************************************/
renameFiles($folder_to_rename,$prefix,$ext);

/***************************************************************************************************
This opens every file (if its extension is found in $ext array) located in specified folder and
searches for each edited file's original name and replaces with edited vesrion.
***************************************************************************/
replaceFileNameIn($folder_to_check_for_renamed_files,$names,$nNames,$ext2);

/***********************************************************************************************
This opens every file (if its extension is found in $ext2 array) located in specified folder and
searches for each edited file's original name and replaces with edited vesrion.
It alsoe adds prefix for 'TARGET' name
************************************************************************************/
replaceFileNames2In($folder_to_check_for_renamed_files,$names,$nNames,$ext2,$prefix);

/*******************************************************************************************************************************
This opens every file if its extension is found in $ext array.
It attempts to replace all renamed TARGET files(e.g. homescreen.exe,aifw.dll) with names with prefix(e.g. hs_app_homescreen.exe);
*************************************************************************************/
replaceFileNames3In($folder_to_check_for_renamed_files,$tnames,$tnNames,$ext);

print '<b>************************************************************************************************************************</b><br/>';
foreach($names as $k=>$v){
  print '<i>'.$v.'</i> <b>=></b> <i>'.$nNames[$k].'</i><br>';    
}

print '<b>************************************************************************************************************************</b><br/>';
foreach($tnames as $k=>$v){
  print '<i>'.$v.'</i> <b>=></b> <i>'.$tnNames[$k].'</i><br>';    
}
display_files($folder_to_check_for_renamed_files);
?>
