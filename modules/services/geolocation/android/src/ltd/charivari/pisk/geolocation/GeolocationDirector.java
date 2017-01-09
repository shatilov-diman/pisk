// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module geolocation of the project pisk.
// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Additional restriction according to GPLv3 pt 7:
// b) required preservation author attributions;
// c) required preservation links to original sources
//
// Original sources:
//   https://github.com/shatilov-diman/pisk/
//   https://bitbucket.org/charivariltd/pisk/
//
// Author contacts:
//   Dmitry Shatilov (e-mail: shatilov.diman@gmail.com; site: https://www.linkedin.com/in/shatilov)
//
//


package ltd.charivari.pisk.geolocation;

import android.util.Log;
import android.os.Bundle;
import android.os.Looper;
import android.content.Context;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;

import java.util.Map;
import java.util.TreeMap;

class GeolocationDirector
{
	static final String TAG = "ltd.charivari.pisk.geolocation";
	static final String tag = "GeolocationDirector: ";

	private Context context;
	private long nativeobjectptr;

	private LocationManager location_manager;
	private Map<String, LocationListener> listeners;

	public GeolocationDirector(Context context, final long nativeobjectptr)
	{
		this.context = context;
		this.nativeobjectptr = nativeobjectptr;
		this.location_manager = (LocationManager)context.getSystemService(android.content.Context.LOCATION_SERVICE);
		this.listeners = new TreeMap<String, LocationListener>();

		if (this.nativeobjectptr == 0)
			throw new IllegalArgumentException();
		if (this.location_manager == null)
			throw new NullPointerException();
		Log.i(TAG, tag + "GeolocationDirector ctor");
	}

	private void detach_native_objectptr()
	{
		for (LocationListener listener : listeners.values())
			this.location_manager.removeUpdates(listener);
		this.listeners.clear();

		if (this.nativeobjectptr == 0)
			throw new IllegalArgumentException();
		this.nativeobjectptr = 0;
	}

	public boolean enable_fine_locate()
	{
		Log.i(TAG, tag + "enable_fine_locate");
		return enable_location(
				LocationManager.GPS_PROVIDER,
				getUpdatePeriodInMs(),
				getUpdateDistance()
		);
	}
	public boolean disable_fine_locate()
	{
		Log.i(TAG, tag + "disable_fine_locate");
		return disable_location(LocationManager.GPS_PROVIDER);
	}

	public boolean enable_coarse_locate()
	{
		Log.i(TAG, tag + "enable_coarse_locate");
		return enable_location(
				LocationManager.NETWORK_PROVIDER,
				getUpdatePeriodInMs(),
				getUpdateDistance()
		);
	}
	public boolean disable_coarse_locate()
	{
		Log.i(TAG, tag + "disable_coarse_locate");
		return disable_location(LocationManager.NETWORK_PROVIDER);
	}

	private boolean enable_location(final String provider, final long update_period, final float update_distance)
	{
		try
		{
			LocationListener listener = new LocationListenerImpl(this.nativeobjectptr);
			this.location_manager.requestLocationUpdates(
				provider, update_period, update_distance, listener, Looper.getMainLooper()
			);
			this.listeners.put(provider, listener);
			return true;
		}
		catch(java.lang.SecurityException ex)
		{
			Log.w(TAG, tag + "failed to request location update, " + ex.getMessage());
		}
		catch(IllegalArgumentException ex)
		{
			Log.w(TAG, tag + "provider does not exists, " + ex.getMessage());
		}
		return false;
	}
	private boolean disable_location(final String provider)
	{
		try
		{
			if (this.listeners.containsKey(provider) == false)
			{
				Log.w(TAG, tag + "GeolocationDirector does not subscribed to provider " + provider);
				return false;
			}
			this.listeners.remove(provider);
			this.location_manager.removeUpdates(this.listeners.get(provider));
			return true;
		}
		catch(Exception ex)
		{
			Log.w(TAG, tag + "fail to remove provider " + provider + ": " + ex.getMessage());
		}
		return false;
	}

	private long getUpdatePeriodInMs()
	{
		return 1000;
	}

	private float getUpdateDistance()
	{
		return 1f;
	}

	private class LocationListenerImpl
		implements LocationListener
	{
		private final long nativeobjectptr;

		public LocationListenerImpl(final long nativeobjectptr)
		{
			this.nativeobjectptr = nativeobjectptr;
		}

		@Override
		public void onProviderEnabled(String provider)
		{
			Log.i(TAG, String.format("Java, onProviderEnabled, provider %s", provider));
			on_provider_enabled(this.nativeobjectptr, provider);
		}
		@Override
		public void onProviderDisabled(String provider)
		{
			Log.i(TAG, String.format("Java, onProviderDisbled, provider %s", provider));
			on_provider_disabled(this.nativeobjectptr, provider);
		}
		@Override
		public void onStatusChanged(String provider, int status, Bundle extra)
		{
			Log.v(TAG, String.format("Java, onStatusChanged, provider %s, status %d", provider, status));
		}
		@Override
		public void onLocationChanged(Location location)
		{
			Log.v(TAG, String.format("Java, onLocationChanged, provider %s, accuracy %f, latitude %f, longitude %f",
				location.getProvider(),
				location.getAccuracy(),
				location.getLatitude(),
				location.getLongitude()
			));
			on_location_changed(
				this.nativeobjectptr,
				location.getProvider(),
				location.getAccuracy(),
				location.getLatitude(),
				location.getLongitude()
			);
		}

		private native void on_provider_enabled(final long nativeobjectptr, final String provider);
		private native void on_provider_disabled(final long nativeobjectptr, final String provider);
		private native void on_location_changed(final long nativeobjectptr, final String provider,
			final double accuracy, final double latitude, final double longitude);
	}
}

