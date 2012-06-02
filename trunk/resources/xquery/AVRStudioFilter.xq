<entity>
{
    for 
        $bitfield in doc($partName)//module[@name=$moduleName]//register//bitfield
    return
        <bitfield offset="{string($bitfield/../@offset)}">
        {
            $bitfield/@*
        }   
            {  
                for $value in $bitfield/../../../value-group[@name=string($bitfield/@values)]/value
                return
                    <value>
                    {
                        $value/@*
                    }
                    </value>
            }
        </bitfield>
}
</entity>
