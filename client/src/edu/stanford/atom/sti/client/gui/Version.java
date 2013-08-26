/** @file Version.java
 *  @author Jason Michael Hogan
 *  @brief Source-file for the class Version
 *  @section license License
 *
 *  Copyright (C) 2008 Jason Hogan <hogan@stanford.edu>\n
 *  This file is part of the Stanford Timing Interface (STI).
 *
 *  The STI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The STI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the STI.  If not, see <http://www.gnu.org/licenses/>.
 */


package edu.stanford.atom.sti.client.gui;

import java.util.Locale;
import java.util.MissingResourceException;
import java.util.PropertyResourceBundle;
import java.util.ResourceBundle;

public class Version {
    
    private double version;
    private String buildNumber;
    private String buildDate;
    private String buildTime;
    
    private final static Locale currentLocale = Locale.getDefault();
    private final static ResourceBundle rb = 
            PropertyResourceBundle.getBundle("edu.stanford.atom.sti.client.resources.stiBuildNumber", currentLocale);

//    private Locale local = new Local(); 
    
    public Version(double versionNumber) {
        version = versionNumber;
        aquireBuildInfo();
    }

    public double getVersionNumber() {
        return version;
    }
    public String getBuildNumber() {
        return buildNumber;
    }
    
    public String getBuildDate() {
        return buildDate;
    }
    public String getBuildTime() {
        return buildTime;
    }
    private void aquireBuildInfo() {
        buildNumber = getResourceToken("BUILD");
        buildDate = getResourceToken("BUILDDATE");
        buildTime = getResourceToken("BUILDTIME");
    }
    
    private static final String getResourceToken(String propertyToken) {
        String msg = "";
        try {
            msg = rb.getString(propertyToken);
        } catch (MissingResourceException e) {
            System.err.println("Token " + propertyToken + " not in Property file!");
        }
        return msg;
    }


}
