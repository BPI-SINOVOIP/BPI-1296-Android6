package com.realtek.DataProvider;

import java.io.File;
import android.util.Log;
import java.util.ArrayList;
import android.net.Uri;

public abstract class AbstractDataProvider
{
    public final static int MAGIC_CODE = 0x01000000;
    public final static int FILE_LIST_CREATE_DONE  = MAGIC_CODE|0x01;
    public final static int FILE_LIST_CREATE_PORTION  = MAGIC_CODE|0x02;
    public final static int FILE_LIST_CREATE_ABORT = MAGIC_CODE|0x10;
	private String mName;
	public static final int NETWORK_DISCONNECT = 0x1;
    private int mSystemState; 
	public void SetName(String name) 
	{ 
		this.mName =name;
	}
	
	public String GetName()
	{	
		return mName;
	}
	 
	public abstract String GetDataAt     (int i);
	public abstract String GetTitleAt    (int i);
	public abstract String GetMimeTypeAt (int i);
	public abstract int    GetIdAt       (int i);
	public abstract int    GetFileTypeAt (int i);
	public abstract int    GetSize       ();
	public abstract File GetFileAt(int i);
	public abstract void sortListByType();
	public abstract int getDirnum();
	public void StopQuery()
	{
	}
	public void UpdateData(int Type)
	{
	}
	public int GetSize(int Type)
	{
		return 0;
	}
    public String GetMediaInfo(int i, int type)
    {
        return null;
    }
    public long GetSizeAt(int i)
	{
		return 0;
	}
    public int GetItemIndex(String item){
        int size = GetSize();
        // Remove prefix "file://"
        if(item.startsWith("file://")){
            item = Uri.decode(item);
            item = item.substring(7);
        }
        //Log.d("RTK","\033[0;33;33m GetItemIndex size:"+size+" \033[m");
        //Log.d("RTK","\033[0;33;33m GetItemIndex item:"+item+" \033[m");
        for(int i=0;i<size;i++){
            //Log.d("RTK","\033[0;33;33m GetItemIndex data:"+GetDataAt(i)+" \033[m");
            String data = GetDataAt(i);
            if(data!= null && data.equals(item))
                return i;
        }
        return -1;
    }
    public void SetSystemState(int state)
    {
        mSystemState = mSystemState |state;
    }
    public void ClearSystemState(int state)
    {
        mSystemState = mSystemState & (~state);
    }
    public boolean IsSystemState(int state)
    {
        if ((mSystemState & state) == state)
            return true;
        else
            return false;
    }
    public ArrayList<String> GetPlayList(int type)
    {
        return null; 
    } 
    public ArrayList<String> GetNameList(int type)
    {
        return null; 
    } 
}
