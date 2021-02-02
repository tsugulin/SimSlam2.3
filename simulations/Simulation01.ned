network Simulation01
{
    submodules: 
        enter1: Create { 
            parameters: 
            	@display("p=50,100;i=block/source");
        } 
        await: Await { 
            parameters: 
            	@display("p=200,100;i=block/circle;q=queue");
        } 
        goon: Goon { 
            parameters: 
            	@display("p=350,100;i=block/process");
        } 
        release1: Release { 
            parameters: 
            	@display("p=500,100;i=block/square");
        } 
        leave: Terminate { 
            parameters:
            	@display("p=650,100;i=block/sink");
        } 
        enter2: Create {
            parameters: 
            	@display("p=50,200;i=block/source");
        } 
        preempt: Preempt { 
            parameters: 
            	@display("p=200,200;i=block/star");
        } 
        release2: Release {
            parameters: 
            	@display("p=500,200;i=block/square");
        } 
        resource: Resource { 
            parameters: 
            	@display("p=350,300;i=block/table");
        } 
    connections:
        enter1.out --> await.in; 
        await.out --> goon.in;
        goon.out --> release1.in;
        release1.out --> leave.in; 
        enter2.out --> preempt.in; 
        preempt.out --> release2.in;
        release2.out --> preempt.in_loop;
        await.res --> resource.from_setup; 
        goon.res --> resource.from_process; 
        release1.res --> resource.from_release1;
        preempt.res --> resource.from_preempt;
        release2.res --> resource.from_release2;
}
