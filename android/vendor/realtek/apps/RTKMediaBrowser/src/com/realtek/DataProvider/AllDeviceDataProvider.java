package com.realtek.DataProvider;

import java.io.*;
import java.net.MalformedURLException;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.RunnableFuture;

import android.util.Log;
import android.os.Handler;

import com.realtek.DLNA_DMP_1p5.DLNABrowser;
import com.realtek.Utils.StorageUtils;

import android.content.Context;
import com.realtek.rtksmb.Computer;
import com.realtek.rtksmb.SubnetScanner;
import jcifs.http.NetworkExplorer;
import jcifs.netbios.NbtAddress;
import jcifs.smb.NtlmPasswordAuthentication;
import jcifs.smb.SmbException;
import jcifs.smb.SmbFile;

public class AllDeviceDataProvider  extends AbstractDataProvider
{	
	private static final String TAG = "AllDeviceDataProvider";
	private static final boolean DEBUG = true;
	private Handler mHandler = null;
    DLNAFileDataProvider mDLNAFileDataProvider = null;
    DLNAFileDataProvider mDLNAFileDataProvider_bg = null;
    SMBFileDataProvider mSMBFileDataProvider = null;
    private ArrayList<String> mUSBDeviceList = new ArrayList<String>();
    private ArrayList<String> mUSBDeviceList_bg = new ArrayList<String>();
    private ArrayList<String> mUSBDeviceList_Title = new ArrayList<String>();
    private ArrayList<String> mUSBDeviceList_Title_bg = new ArrayList<String>();
    private ArrayList<Computer> mSMBDeviceList = new ArrayList<>();
    private ArrayList<Computer> mSMBDeviceList_bg = new ArrayList<>();
    private ArrayList<String> mSMBDomainList_bg = new ArrayList<String>();
    private int mSMBDomainSize = 0;
    private int mDMSSize = 0;
    private int mDMSSize_bg = 0;
	private Context mContext;

    public static final int USB_QUERY_DONE = 1;
    public static final int DMS_QUERY_DONE = 2;
    public static final int SMB_QUERY_DOMAIN_DONE = 4;
    public static final int SMB_QUERY_ALL_DOMAIN_DONE = 8;
    public static final int DMS_QUERY_ABORT = 16;
    public static final int SMB_QUERY_DOMAIN_ABORT = 32;

	public static final int UPDATE_QUERY_USB = 1;	
	public static final int UPDATE_QUERY_DMS = 2;	
	public static final int UPDATE_QUERY_SMB = 4;	
	public static final int UPDATE_SMB = 8;	
    private Handler mDLNAHandler = new Handler();
    
	private static boolean StopDevQuery = true;
	private static final Object lock = new Object();
	private SubnetScanner subnetScanner;

	public AllDeviceDataProvider (Handler handler, Context context, int type)
	{
		super.SetName("AllDeviceDataProvider");
		
	    mHandler = handler;
		mContext = context;
	    CreateFileListArray(type);
		
	}
	
	public String GetDataAt(int i)
	{

	    synchronized(mSMBDeviceList) 
        {   
           
    		if (i < mUSBDeviceList.size())
            {
                return mUSBDeviceList.get(i);
            }
            else if (i < (mUSBDeviceList.size() + mDMSSize))
            {
                return mDLNAFileDataProvider.GetDataAt(i - mUSBDeviceList.size());
            }
            else if (i < GetSize())
            {
                return "smb://"+mSMBDeviceList.get(i - mUSBDeviceList.size() - mDMSSize).addr+"/";
            }
        }
        return null;
	}

	public File GetFileAt(int i)
	{
		return null;
	}

	public String GetTitleAt    (int i)
	{

	    synchronized(mSMBDeviceList) 
        {
    		if (i < mUSBDeviceList.size())
            {
				return mUSBDeviceList_Title.get(i);
            }
            else if (i < (mUSBDeviceList.size()+ mDMSSize))
            {
                return mDLNAFileDataProvider.GetTitleAt(i - mUSBDeviceList.size());
            }
            else if (i < GetSize())
            {
				String smbTitle = mSMBDeviceList.get(i - mUSBDeviceList.size() - mDMSSize).name;
				if(smbTitle==null)
					smbTitle=mSMBDeviceList.get(i - mUSBDeviceList.size() - mDMSSize).addr;
                String[] segs = smbTitle.split("/");
                if (segs.length ==0)
                {
                    Log.d(TAG,"ERROR String["+mSMBDeviceList.get(i - mUSBDeviceList.size() - mDMSSize)+"]");
                }
                return segs[segs.length - 1];
            }
        }
        return null;
	}
	
	public  String GetMimeTypeAt (int i)
	{
		return null;
	}
	
	public int GetIdAt(int i)
	{
		
		return -1;
	}
	
	public int GetSize ()
	{
        synchronized(mSMBDeviceList) 
        {   
            return mUSBDeviceList.size() + mDMSSize + mSMBDeviceList.size();
        }
    }
	public int GetSize(int type)
	{
		if (type==FileFilterType.DEVICE_FILE_DEVICE)
		{
			return mUSBDeviceList.size();
		}
		else if (type==FileFilterType.DEVICE_FILE_DLNA_DEVICE)
		{
			return mDMSSize;
		}
		else if (type==FileFilterType.DEVICE_FILE_SMB_DEVICE)
		{
			return mSMBDeviceList.size(); 
		}
		else
		{
			return 0;
		}
	}
	
	public int GetFileTypeAt(int i)
	{
	    synchronized(mSMBDeviceList) 
        {   
    	    if (i < mUSBDeviceList.size())
            {
                return FileFilterType.DEVICE_FILE_DEVICE;
            }
            else if (i < (mUSBDeviceList.size() + mDMSSize))
            {
                return FileFilterType.DEVICE_FILE_DLNA_DEVICE;
            }
            else if (i < GetSize())
            {
                return FileFilterType.DEVICE_FILE_SMB_DEVICE;
            }
		    return FileFilterType.DEVICE_FILE_NONE;
       }
	}
	public void UpdateData(int Type)
	{
		synchronized(lock)
		{
			StopDevQuery = false;
		}
		if ((Type & UPDATE_QUERY_USB) ==UPDATE_QUERY_USB)
			CreateUSBDeviceArray();	
		if ((Type & UPDATE_QUERY_DMS) ==UPDATE_QUERY_DMS)
			CreateDMSDeviceArray();	
 
		if ((Type & UPDATE_QUERY_SMB) ==UPDATE_QUERY_SMB)
			CreateSMBDeviceArray(true);	
		else if ((Type & UPDATE_SMB) ==UPDATE_SMB)
			CreateSMBDeviceArray(false);	

	}
	private void CreateFileListArray(int Type)
	{
		synchronized(lock)
		{
			StopDevQuery = false;
		}
		if ((Type & UPDATE_QUERY_USB) ==UPDATE_QUERY_USB)
			CreateUSBDeviceArray();	
		if ((Type & UPDATE_QUERY_DMS) ==UPDATE_QUERY_DMS)
			CreateDMSDeviceArray();	
 
		if ((Type & UPDATE_QUERY_SMB) ==UPDATE_QUERY_SMB)
			CreateSMBDeviceArray(true);	
	}
	private void CreateUSBDeviceArray()
	{
        //USB Device
		//Update latest dev list to fordward 
		Log.d(TAG,"CreateUSBDeviceArray Start");
		if (mUSBDeviceList!= null)
		{
			mUSBDeviceList.clear();
			mUSBDeviceList_Title.clear();
		}
        mUSBDeviceList = new ArrayList<String>(mUSBDeviceList_bg);
        mUSBDeviceList_Title = new ArrayList<String>(mUSBDeviceList_Title_bg);
        mUSBDeviceList_bg.clear();
        mUSBDeviceList_Title_bg.clear();
		mDLNAHandler.post(new Runnable(){
        public void run()
		{
			List<StorageUtils.StorageInfo> storageList = StorageUtils.getStorageList(mContext);
			for(StorageUtils.StorageInfo info : storageList){
				mUSBDeviceList_bg.add(info.path);
				mUSBDeviceList_Title_bg.add(info.getDisplayName());
			}

			if (!StopDevQuery)
			{
       			mHandler.sendEmptyMessage(USB_QUERY_DONE);
			}
		}
		});
	}
	private void CreateDMSDeviceArray()	
	{
        //DMS Device
		Log.d(TAG, "CreateDMSDeviceArray Start");
		if (mDLNAFileDataProvider_bg != null)
		{
			mDLNAFileDataProvider = mDLNAFileDataProvider_bg;
			mDLNAFileDataProvider_bg = null;
			mDMSSize = mDMSSize_bg;
		}
        else
        {
            mDLNAFileDataProvider = null;
            mDMSSize = 0;
        }
        mDLNAHandler.post( new Runnable(){
                public void run(){                    
                    //DLNA Device
                    mDLNAFileDataProvider_bg =  new DLNAFileDataProvider(DLNABrowser.GetInstance().GetPluginRootPath(), 
                    FileFilterType.DEVICE_FILE_DIR,
                    -1, 0, null);
                    mDLNAFileDataProvider_bg.UpdateData(0);
                    mDMSSize_bg = mDLNAFileDataProvider_bg.GetSize();
                    if (IsSystemState(NETWORK_DISCONNECT))
                    {
                        mDLNAFileDataProvider_bg = null;
                        mHandler.sendEmptyMessage(DMS_QUERY_ABORT);
                    }
					else if (!StopDevQuery)
					{
                    	mHandler.sendEmptyMessage(DMS_QUERY_DONE);
					}
                }
            });
	}

	private static void log(String s){
		if(DEBUG)
			Log.d(TAG, s);
	}

	private void CreateSMBDeviceArray(boolean QueryAgain)
	{

        //SMB Device
		Log.d(TAG,"CreateSMBDeviceArray Start QueryAgain:"+QueryAgain);
	    synchronized(mSMBDeviceList)
        { 
	        if (mSMBDeviceList!= null)
	        {
	        	mSMBDeviceList.clear();
	        }
	        mSMBDeviceList = new ArrayList<Computer>(mSMBDeviceList_bg);
        }
		if (QueryAgain)
		{
			final int method = 3;
			if(method==3) {
				log("subnetScanner method");
				subnetScanner = new SubnetScanner(mContext);
				subnetScanner.setObserver(new SubnetScanner.ScanObserver() {
					@Override
					public void computerFound(final Computer computer) {
						if(computer.name!=null && computer.name.equals(NbtAddress.MASTER_BROWSER_NAME)) return;
//						log("computerFound:"+computer.name+","+computer.addr);
						if(!mSMBDeviceList_bg.contains(computer))
							mSMBDeviceList_bg.add(computer);
						mHandler.sendEmptyMessage(SMB_QUERY_DOMAIN_DONE);
					}

					@Override
					public void searchFinished() {
						mHandler.sendEmptyMessage(SMB_QUERY_ALL_DOMAIN_DONE);
					}
				});
				mHandler.postDelayed(new Runnable() {
					@Override
					public void run() {
						subnetScanner.start();
					}
				}, 100);
			} else {
				Thread thread = new Thread( new Runnable(){
					public void run(){
						if(method==2) {
							//JCIFS method
							log("JCIFS method");
							mSMBDeviceList_bg.clear();
							SmbFile[] domains = null;
							try {
								SmbFile smbRoot = new SmbFile("smb://", NtlmPasswordAuthentication.ANONYMOUS);
								domains = smbRoot.listFiles();
							} catch (MalformedURLException e) {
								e.printStackTrace();
							} catch (SmbException e) {
								e.printStackTrace();
							}
//						mSMBDomainList_bg.clear();
							if(domains==null) return;
							log("domain count="+domains.length);
							for (SmbFile d : domains) {
								String domainName = d.getName();
								log("domain name="+domainName);
//								mSMBDomainList_bg.add(f.getName());
								SmbFile[] machines = null;
								try {
									machines = d.listFiles();
								}catch(SmbException e){
									e.printStackTrace();
								}
								if(machines==null) continue;
								log("machine count="+machines.length);
								for (SmbFile m : machines) {
									log("machine name="+m.getName());
									if (StopDevQuery || IsSystemState(NETWORK_DISCONNECT)) {
										Log.d(TAG, "Let's get out SMB query[1]");
										break;
									}
									mSMBDeviceList_bg.add(new Computer("//"+domainName+m.getName(), null));
								}
								mHandler.sendEmptyMessage(SMB_QUERY_DOMAIN_DONE);
							}
							if (IsSystemState(NETWORK_DISCONNECT)) {
								mSMBDeviceList_bg.clear();
								mSMBDomainList_bg.clear();
								mHandler.sendEmptyMessage(SMB_QUERY_DOMAIN_ABORT);
							} else if (!StopDevQuery) {
								//Send message for All DONAIN is queried done.
								mHandler.sendEmptyMessage(SMB_QUERY_ALL_DOMAIN_DONE);
							}
						}else {
							//Query All Domain
							mSMBFileDataProvider = new SMBFileDataProvider("//", 0, -1, 0, null, mContext, null); //Get All domains
							mSMBDeviceList_bg.clear();
							mSMBDomainList_bg.clear();
							mSMBDomainSize = mSMBFileDataProvider.GetSize();
							for (int i = 0; i < mSMBDomainSize; i++) {
								log("SMBDomain get:" + mSMBFileDataProvider.GetDataAt(i));
								mSMBDomainList_bg.add(new String(mSMBFileDataProvider.GetDataAt(i)));
							}
							//Query All Server for each domain.
							for (int i = 0; i < mSMBDomainSize; i++) {
								if (StopDevQuery || IsSystemState(NETWORK_DISCONNECT)) {
									Log.d(TAG, "Let's get out SMB query[1]");
									break;
								}
								//Log.d(TAG,"CreateSMBDeviceArray [1]");
								if (mSMBDomainList_bg.get(i).equals("//")) {
									Log.d(TAG, "WARNING!!!! We got empty domain, and we will not query this domain");
									continue;
								}
								mSMBFileDataProvider = new SMBFileDataProvider(mSMBDomainList_bg.get(i), 0, -1, 0, null, mContext, null); //Get All Servers
								//Log.d(TAG,"CreateSMBDeviceArray [2]");
								int serverSize = mSMBFileDataProvider.GetSize();
								if (serverSize > 0) {
									synchronized (mSMBDeviceList_bg) {
										for (int j = 0; j < serverSize; j++) {
											if (StopDevQuery || IsSystemState(NETWORK_DISCONNECT)) {
												Log.d(TAG, "Let's get out SMB query[2]");
												break;
											}
											log("mSMBFileDataProvider.GetDataAt(j)="+mSMBFileDataProvider.GetDataAt(j));
											mSMBDeviceList_bg.add(new Computer(mSMBFileDataProvider.GetDataAt(j), null));
										}
									}
									//Log.d(TAG,"CreateSMBDeviceArray size="+serverSize);
									mHandler.sendEmptyMessage(SMB_QUERY_DOMAIN_DONE);
								}
							}
							if (IsSystemState(NETWORK_DISCONNECT)) {
								mSMBDeviceList_bg.clear();
								mSMBDomainList_bg.clear();
								mHandler.sendEmptyMessage(SMB_QUERY_DOMAIN_ABORT);
							} else if (!StopDevQuery) {
								//Send message for All DONAIN is queried done.
								mHandler.sendEmptyMessage(SMB_QUERY_ALL_DOMAIN_DONE);
							}
						}
					}
				}
				);
				thread.start();
			}
		}
		Log.d(TAG,"CreateSMBDeviceArray End QueryAgain:"+QueryAgain);
	}
	public void StopQuery()
	{
		synchronized(lock)
		{
			StopDevQuery = true;
		}
		if(subnetScanner!=null) {
			subnetScanner.interrupt();
			subnetScanner = null;
		}
	}

	public int getDirnum() {
		return 0;
	}

	public void sortListByType() {
	}

	public void clearNetworkItems(){
		if(mSMBDeviceList!=null){
			mSMBDeviceList.clear();
		}
		if(mSMBDeviceList_bg!=null){
			mSMBDeviceList_bg.clear();
		}
		mDLNAFileDataProvider = null;
		mDLNAFileDataProvider_bg = null;
		mDMSSize = 0;
		mDMSSize_bg = 0;
	}
}
