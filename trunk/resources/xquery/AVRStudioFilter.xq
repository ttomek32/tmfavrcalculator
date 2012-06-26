<entity>
    <fusebits>
    {
        for 
            $fuse in doc($partName)//module[@name='FUSE']//register//bitfield
        return
            <fuse offset="{string($fuse/../@offset)}" registername="{string($fuse/../@name)}">
            {
                $fuse/@*
            }
                {
                    for $value in $fuse/../../../value-group[@name=string($fuse/@values)]/value
                    return
                        <value>
                        {
                            $value/@*
                        }
                        </value>
                }
            </fuse>
    }
    </fusebits>
    <lockbits>
    {
        for 
            $lock in doc($partName)//module[@name='LOCKBIT']//register//bitfield
        return
            <fuse offset="{string($lock/../@offset)}">
            {
                $lock/@*
            }   
                {
                    for $value in $lock/../../../value-group[@name=string($lock/@values)]/value
                    return
                        <value>
                        {
                            $value/@*
                        }
                        </value>
                }
            </fuse>
    }
    </lockbits>
    <interfaces>
    {
        for 
            $interface in doc($partName)//interfaces/interface
        return
            <interface>
                {$interface/@*}
            </interface>
    }
    </interfaces>
</entity>
